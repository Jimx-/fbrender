#ifndef _RENDER_DEVICE_H_
#define _RENDER_DEVICE_H_

#include "transform.h"
#include "vertex.h"

namespace fbrender {

    class RenderDevice {
    public:
        static const int DS_WIREFRAME = 0x1;
        static const int DS_COLOR = 0x2;
        static const int DS_LIGHTING = 0x4;
        static const int DS_TEXTURE_2D = 0x8;

        static const int CF_RGB = 0x1;
        static const int CF_RGBA = 0x2;

        static const int TF_NEAREST = 0x1;

        RenderDevice() 
        { 
            initialized = false; 
            framebuffer[0] = framebuffer[1] = nullptr;
            pixel_buffer = nullptr;
            zbuffer = nullptr;
            texbuffer = nullptr;
        }

        ~RenderDevice();

        bool ready() { return initialized; }

        void set_world(const Matrix4& mat) { transform.set_world(mat); light_world_pos = light_pos * mat; }
        void set_camera(const Vector4& pos, const Vector4& at, const Vector4& up);
        void set_projection(const Matrix4& mat) { transform.set_projection(mat); }

        void set_light_pos(const Vector4& pos) { light_pos = pos; light_world_pos = light_pos * transform.get_world(); }
        void set_light_ambient(const Color& amb) { ambient_color = amb; }
        void set_light_diffuse(const Color& diff) { diffuse_color = diff; }
        void set_light_specular(const Color& spec) { specular_color = spec; }

        void set_material_ambient(const Color& amb) { material_ambient = amb; }
        void set_material_diffuse(const Color& diff) { material_diffuse = diff; }
        void set_material_specular(const Color& spec) { material_specular = spec; }
        void set_material_emission(const Color& emi) { material_emission = emi; }
        void set_shininess(Real shi) { material_shininess = shi; }

        void texture_image_2d(int width, int height, int format, const void* tex);
        void set_texture_filter(int filter) { tex_filter = filter; }

        void enable(int state) { drawing_state |= state; }
        void disable(int state) { drawing_state &= ~state; }

        void clear();
        void clear_color(const Color& color);
        void draw_pixel(int x, int y, uint32_t color);
        void draw_line(int x1, int y1, int x2, int y2, uint32_t color);
        void draw_triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);
        void swap_buffers();
    private:
        Transform transform;
        int width;
        int height;

        uint32_t* pixel_buffer;
        uint32_t** framebuffer[2];
        size_t framebuffer_size;
        int buffer_index;

        Real** zbuffer;
        int drawing_state;
        bool initialized;

        uint32_t** texbuffer;
        int tex_filter;
        int tex_width;
        int tex_height;

        uint32_t background;
        uint32_t foreground;

        Vector4 camera_pos;
        Vector4 light_pos;
        Vector4 light_world_pos;
        /* light colors */
        Color ambient_color;
        Color diffuse_color;
        Color specular_color;

        Color material_ambient;
        Color material_diffuse;
        Color material_specular;
        Color material_emission;
        Real material_shininess;

        bool back_face_test(const Vertex& v1, const Vertex& v2, const Vertex& v3);

        void rasterize_triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);
        void draw_triangle_top(const Vertex& v1, const Vertex& v2, const Vertex& v3);
        void draw_triangle_bottom(const Vertex& v1, const Vertex& v2, const Vertex& v3);
        void draw_scan_line(const Vertex& left, const Vertex& right, int y_index);

        void lighting(Vertex& v, const Vector4& normal);

        void clear_texbuffer();
    protected:
        void init(int width, int height);

        virtual void copy_buffer(const void* buffer, size_t size) = 0;
    };
}

#endif

