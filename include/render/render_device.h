#ifndef _RENDER_DEVICE_H_
#define _RENDER_DEVICE_H_

#include "transform.h"
#include "vertex.h"

namespace fbrender {

    class RenderDevice {
    public:
        static const int DS_WIREFRAME = 0x1;
        static const int DS_COLOR = 0x2;

        RenderDevice() 
        { 
            initialized = false; 
            framebuffer = nullptr;
            zbuffer = nullptr;
        }

        ~RenderDevice();

        bool ready() { return initialized; }

        void set_world(const Matrix4& mat) { transform.set_world(mat); }
        void set_view(const Matrix4& mat) { transform.set_view(mat); }
        void set_projection(const Matrix4& mat) { transform.set_projection(mat); }
        void set_drawing_state(int state) { drawing_state = state; }

        void clear();
        void draw_pixel(int x, int y, uint32_t color);
        void draw_line(int x1, int y1, int x2, int y2, uint32_t color);
        void draw_triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);
    private:
        Transform transform;
        int width;
        int height;
        uint32_t** framebuffer;
        Real** zbuffer;
        int drawing_state;
        bool initialized;

        uint32_t background;
        uint32_t foreground;

        bool back_face_test(const Vertex& v1, const Vertex& v2, const Vertex& v3);

        void rasterize_triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);
        void draw_triangle_top(const Vertex& v1, const Vertex& v2, const Vertex& v3);
        void draw_triangle_bottom(const Vertex& v1, const Vertex& v2, const Vertex& v3);
        void draw_scan_line(const Vertex& left, const Vertex& right, int y_index);
    protected:
        void init(int width, int height, void* fb);
    };
}

#endif

