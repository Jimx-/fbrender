#include "render/render_device.h"

#include <vector>
#include <algorithm>
#include <sys/mman.h>
#include <iostream>
using namespace std;

namespace fbrender {
    
    void RenderDevice::init(int width, int height)
    {
        /* allocate framebuffer and z-buffer */
        framebuffer_size = height * width * sizeof(uint32_t);

        delete framebuffer[0];
        delete framebuffer[1];
        delete [] pixel_buffer;
        delete [] zbuffer;

        framebuffer[0] = new uint32_t*[height];
        framebuffer[1] = new uint32_t*[height];
        zbuffer = new Real*[height];

        pixel_buffer = (uint32_t *) mmap (0, framebuffer_size * 2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0); 
        if (pixel_buffer == MAP_FAILED) {
            pixel_buffer = nullptr;
        }

        char* pfb = (char*) pixel_buffer;
        for (int i = 0; i < height; i++) {
            framebuffer[0][i] = (uint32_t*)(pfb + width * sizeof(uint32_t) * i);
            zbuffer[i] = new Real[width]();
        }
        pfb += framebuffer_size;
        for (int i = 0; i < height; i++) {
            framebuffer[1][i] = (uint32_t*)(pfb + width * sizeof(uint32_t) * i);
        }

        buffer_index = 0;

        this->width = width;
        this->height = height;

        transform = Transform(width, height);
        background = 0;
        foreground = 0xffffffff;

        /* lighting parameter */
        light_pos = {50, 0, 0};

        /* texture parameter */
        tex_filter = TF_NEAREST;

        drawing_state = 0;
    }

    RenderDevice::~RenderDevice()
    {
        delete framebuffer[0];
        delete framebuffer[1];

        if (zbuffer) {
            for (int i = 0; i < height; i++) {
                if (zbuffer[i]) delete [] zbuffer[i];
            }
        }
        delete [] zbuffer;

        clear_texbuffer();

        if (pixel_buffer) munmap(pixel_buffer, framebuffer_size * 2);
    }

    void RenderDevice::clear()
    {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                framebuffer[buffer_index][i][j] = background;
                zbuffer[i][j] = 0.0;
            }
        }
    }

    void RenderDevice::clear_color(const Color& c)
    {
        background = c.color_value();
    }

    void RenderDevice::set_camera(const Vector4& pos, const Vector4& at, const Vector4& up)
    {
        transform.set_view(Matrix4::lookat(pos, at, up));
        camera_pos = pos;
        camera_world_pos = camera_pos * transform.get_world();
    }

    void RenderDevice::clear_texbuffer()
    {
        if (texbuffer) {
            for (int i = 0; i < tex_height; i++) {
                if (texbuffer[i]) delete [] texbuffer[i];
            }
        }
        delete [] texbuffer;
        texbuffer = nullptr;
    }

    void RenderDevice::texture_image_2d(int width, int height, int format, const void* tex)
    {
        if (texbuffer) clear_texbuffer();

        char* tp = (char*)tex;
        if (width <= 0 || height <= 0) return;
        texbuffer = new uint32_t*[height];

        size_t size;
        switch (format) {
            case CF_RGB:
                size = 3;
                break;
            case CF_RGBA:
                size = 4;
                break;
        }

        for (int i = 0; i < height; i++) {
            uint32_t* line = new uint32_t[width];
            for (int j = 0; j < width; j++) {
                line[j] = *(uint32_t*)tp;
                tp += size;
            }

            texbuffer[i] = line;
        }

        tex_height = height;
        tex_width = width;
    }

    void RenderDevice::swap_buffers()
    {
        char* pfb = (char*)pixel_buffer;
        pfb += buffer_index * framebuffer_size;
        copy_buffer(pfb, framebuffer_size);

        buffer_index = 1 - buffer_index;
    }

    void RenderDevice::draw_pixel(int x, int y, uint32_t color)
    {
        if (x < width && y < height) {
            framebuffer[buffer_index][y][x] = color;
        }
    }

    void RenderDevice::draw_line(int x1, int y1, int x2, int y2, uint32_t color)
    {
        /* Bresenham algorithm */
        if (x1 == x2 && y1 == y2) {
            draw_pixel(x1, y1, color);
        } else if (x1 == x2) {
            int step = y2 > y1 ? 1 : -1;
            for (int i = y1; i != y2; i += step) draw_pixel(x1, i, color);
        } else if (y1 == y2) {
            int step = x2 > x1 ? 1 : -1;
            for (int j = x1; j != x2; j += step) draw_pixel(j, y1, color);
        } else {
            int dx = x2 - x1;
            int dy = y2 - y1;
            int ux = ((dx > 0) << 1) - 1;
            int uy = ((dy > 0) << 1) - 1;
            int x = x1, y = y1, eps;

            eps = 0;
            dx = (dx > 0) ? dx : -dx; 
            dy = (dy > 0) ? dy : -dy; 
            if (dx > dy) {
                for (x = x1; x != x2; x += ux) {
                    draw_pixel(x, y, color);
                    eps += dy;
                    if ((eps << 1) >= dx) {
                        y += uy; 
                        eps -= dx;
                    }
                }
            } else {
                for (y = y1; y != y2; y += uy) {
                    draw_pixel(x, y, color);
                    eps += dx;
                    if ((eps << 1) >= dy) {
                        x += ux; eps -= dy;
                    }
                }
            }    
        }
    }

    void RenderDevice::draw_triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3)
    {
        Vertex p1 = v1;
        Vertex p2 = v2;
        Vertex p3 = v3;

        Vector4 edge1 = p2.get_pos() - p1.get_pos();
        Vector4 edge2 = p3.get_pos() - p2.get_pos();

        Vector4 _normal = edge1.cross_product(edge2);
        const Matrix4& mw = transform.get_world();
        Vector4 normal = _normal * mw.inverse().transpose();
        p1.set_normal(normal);
        p2.set_normal(normal);
        p3.set_normal(normal);

        p1 = transform.apply_mv_transform(p1); 
        p2 = transform.apply_mv_transform(p2); 
        p3 = transform.apply_mv_transform(p3);

        if (!back_face_test(p1, p2, p3)) return;

        Vertex c1 = transform.apply_projection(p1);
        Vertex c2 = transform.apply_projection(p2); 
        Vertex c3 = transform.apply_projection(p3); 

        if (transform.check_cvv(c1)) return;
        if (transform.check_cvv(c2)) return;
        if (transform.check_cvv(c3)) return;

        p1 = transform.homogenize(c1);
        p2 = transform.homogenize(c2);
        p3 = transform.homogenize(c3);

        if (drawing_state & (DS_COLOR | DS_TEXTURE_2D)) {
            rasterize_triangle(p1, p2, p3);  
        } 
        if (drawing_state & DS_WIREFRAME) {
            const Vector4& pos1 = p1.get_pos();
            const Vector4& pos2 = p2.get_pos();
            const Vector4& pos3 = p3.get_pos();

            draw_line(pos1.x, pos1.y, pos2.x, pos2.y, foreground);
            draw_line(pos1.x, pos1.y, pos3.x, pos3.y, foreground);
            draw_line(pos2.x, pos2.y, pos3.x, pos3.y, foreground);
        }
    }

    bool RenderDevice::back_face_test(const Vertex& v1, const Vertex& v2, const Vertex& v3)
    {
        if (drawing_state & DS_WIREFRAME) {
            return true;
        }

        Vector4 vec1 = v2.get_pos() - v1.get_pos();
        Vector4 vec2 = v3.get_pos() - v2.get_pos();

        Vector4 normal = vec1.cross_product(vec2);
        Vector4 dir = v1.get_pos();
        Real dot = normal.dot_product(dir);

        return dot > 0;
    }

    void RenderDevice::rasterize_triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3)
    {
        const Vector4& p1 = v1.get_pos();
        const Vector4& p2 = v2.get_pos();
        const Vector4& p3 = v3.get_pos();

        if (p1.y == p2.y) {
            if (p1.y < p3.y) {
                draw_triangle_top(v1, v2, v3);
            } else {
                draw_triangle_bottom(v3, v1, v2);
            }
        } else if (p1.y == p3.y) {
            if (p1.y < p2.y) {
                draw_triangle_top(v1, v3, v2);
            } else {
                draw_triangle_bottom(v2, v1, v3);
            }
        } else if (p2.y == p3.y) {
            if (p2.y < p1.y) {
                draw_triangle_top(v2, v3, v1);
            } else {
                draw_triangle_bottom(v1, v2, v3);
            }
        } else {
            /* sort vertexes by y */
            std::vector<Vertex> v = { v1, v2, v3 };
            sort(v.begin(), v.end(), [](const Vertex& v1, const Vertex& v2) { return v1.get_pos().y < v2.get_pos().y; });

            Vertex& top = v[0];
            Vertex& middle = v[1];
            Vertex& bottom = v[2];

            const Vector4& tp = top.get_pos();
            const Vector4& mp = middle.get_pos();
            const Vector4& bp = bottom.get_pos();

            Real ratio = (mp.y - tp.y) / (bp.y - tp.y);
            Real middle_x = ratio * (bp.x - tp.x) + tp.x;

            Vertex new_middle(middle_x, mp.y, 0, 0, 0, 0, 0, 0, 0); 
            new_middle.lerp(top, bottom, ratio);

            draw_triangle_bottom(top, new_middle, middle);
            draw_triangle_top(new_middle, middle, bottom);
        }
    }

#define ROUND_AWAY_FROM_ZERO(x) (int)(((x) < 0) ? floor(x) : ceil(x))
    void RenderDevice::draw_triangle_top(const Vertex& v1, const Vertex& v2, const Vertex& v3)
    {
        const Vector4& p1 = v1.get_pos();
        const Vector4& p2 = v2.get_pos();
        const Vector4& p3 = v3.get_pos();

        for (Real y = p1.y; y <= p3.y; y += (Real)0.5) {
            int yi = ROUND_AWAY_FROM_ZERO(y);

            if (yi >= 0 && yi < height) {
                Real ratio = (y - p1.y) / (p3.y - p1.y);

                Real lx = ratio * (p3.x - p1.x) + p1.x;
                Real rx = ratio * (p3.x - p2.x) + p2.x;

                Vertex n1(lx, y, 0, 0, 0, 0, 0, 0, 0);
                n1.lerp(v1, v3, ratio);
                Vertex n2(rx, y, 0, 0, 0, 0, 0, 0, 0);
                n2.lerp(v2, v3, ratio);

                const Vector4& np1 = n1.get_pos();
                const Vector4& np2 = n2.get_pos();
                if (np1.x < np2.x) {
                    draw_scan_line(n1, n2, yi);
                } else {
                    draw_scan_line(n2, n1, yi);
                }
            }
        }
}
    void RenderDevice::draw_triangle_bottom(const Vertex& v1, const Vertex& v2, const Vertex& v3)
    {
        const Vector4& p1 = v1.get_pos();
        const Vector4& p2 = v2.get_pos();
        const Vector4& p3 = v3.get_pos();

        for (Real y = p1.y; y <= p3.y; y += (Real)0.5) {
            int yi = ROUND_AWAY_FROM_ZERO(y);

            if (yi >= 0 && yi < height) {
                Real ratio = (y - p1.y) / (p2.y - p1.y);

                Real lx = ratio * (p2.x - p1.x) + p1.x;
                Real rx = ratio * (p3.x - p1.x) + p1.x;

                Vertex n1(lx, y, 0, 0, 0, 0, 0, 0, 0);
                n1.lerp(v1, v2, ratio);
                Vertex n2(rx, y, 0, 0, 0, 0, 0, 0, 0);
                n2.lerp(v1, v3, ratio);

                const Vector4& np1 = n1.get_pos();
                const Vector4& np2 = n2.get_pos();
                if (np1.x < np2.x) {
                    draw_scan_line(n1, n2, yi);
                } else {
                    draw_scan_line(n2, n1, yi);
                }
            }
        }
    }

    void RenderDevice::draw_scan_line(const Vertex& left, const Vertex& right, int y_index)
    {
        const Vector4& lp = left.get_pos();
        const Vector4& rp = right.get_pos();
        const TexCoord& lvt = left.get_texcoord();
        const TexCoord& rvt = right.get_texcoord();
        const Color& lvc = left.get_color();
        const Color& rvc = right.get_color();
        const Vector4& lwp = left.get_world_pos();
        const Vector4& rwp = right.get_world_pos();
        const Vector4& lnm = left.get_normal();
        const Vector4& rnm = right.get_normal();
        
        Real dx = rp.x - lp.x;
        for (Real x = lp.x; x <= rp.x; x += (Real)0.5) {
            int x_index = (int)(x + 0.5);
        
            if (x_index >= 0 && x_index < width) {
                Real t = 0;
                if (dx != 0) {
                    t = (x - lp.x) / dx;
                }

#define LERP(a, b, t) ((b) * (t) + (1 - (t)) * (a))
                Real invw = LERP(left.get_one_per_w(), right.get_one_per_w(), t);
                if (invw >= zbuffer[y_index][x_index]) {
                    Real w = 1 / invw;
                    zbuffer[y_index][x_index] = invw;

                    Real u = LERP(lvt.u, rvt.u, t) * w * (tex_width - 1);
                    Real v = LERP(lvt.v, rvt.v, t) * w * (tex_height - 1);

                    Color tex_color;
                    if (drawing_state & DS_TEXTURE_2D) {
                        if (tex_filter == TF_NEAREST) {
                            int ui = ROUND_AWAY_FROM_ZERO(u);
                            int vi = ROUND_AWAY_FROM_ZERO(v);
                            
                            ui = ui <= 0 ? 0 : ui >= tex_width ? (tex_width - 1) : ui;
                            vi = vi <= 0 ? 0 : vi >= tex_height ? (tex_height - 1) : vi;
                            tex_color = Color(texbuffer[vi][ui]);
                        }
                    }

                    Color vcolor = Color(LERP(lvc.r, rvc.r, t) * w,
                            LERP(lvc.g, rvc.g, t) * w,
                            LERP(lvc.b, rvc.b, t) * w);

                    Vector4 world_pos = Vector4::lerp(lwp, rwp, t) * w;
                    Vector4 normal = Vector4::lerp(lnm, rnm, t) * w;

                    normal.normalize(); 

                    Vector4 light_dir = world_pos - light_world_pos;
                    light_dir.normalize();

                    Real kdiffuse = light_dir.dot_product(normal);
                    if (kdiffuse < 0) kdiffuse = 0;

                    Color diffuse = material_diffuse * kdiffuse + diffuse_color * kdiffuse;

                    Color lcolor = diffuse + ambient_color;

                    if (drawing_state & DS_LIGHTING) {
                        vcolor = vcolor * lcolor;
                        tex_color = tex_color * lcolor;
                    }

                    if (drawing_state & DS_COLOR) {
                        draw_pixel(x_index, y_index, vcolor.color_value());
                    } else if (drawing_state & DS_TEXTURE_2D) {
                        draw_pixel(x_index, y_index, tex_color.color_value());
                    }
                }
            }
        }
    }

}

