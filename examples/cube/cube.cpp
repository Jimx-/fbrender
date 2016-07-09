#include "render/fb_render_device.h"

#include <time.h>
#include <unistd.h>

using namespace fbrender;

Vertex mesh[] = {
    {   1, -1,  1, 1, 0, 0, 1.0f, 0.2f, 0.2f  },
	{  -1, -1,  1, 1, 0, 1, 0.2f, 1.0f, 0.2f  },
	{  -1,  1,  1, 1, 1, 1, 0.2f, 0.2f, 1.0f  },
	{   1,  1,  1, 1, 1, 0, 1.0f, 0.2f, 1.0f  },
	{   1, -1, -1, 1, 0, 0, 1.0f, 1.0f, 0.2f  },
	{  -1, -1, -1, 1, 0, 1, 0.2f, 1.0f, 1.0f  },
	{  -1,  1, -1, 1, 1, 1, 1.0f, 0.3f, 0.3f  },
	{   1,  1, -1, 1, 1, 0, 0.2f, 1.0f, 0.3f  }, 
};

void draw_plane(RenderDevice* device, int a, int b, int c, int d)
{
    Vertex v1 = mesh[a], v2 = mesh[b], v3 = mesh[c], v4 = mesh[d];
    device->draw_triangle(v1, v2, v3);
    device->draw_triangle(v3, v4, v1);
} 

void draw_box(RenderDevice* device, Real theta)
{
    device->set_world(Matrix4::rotate(-1, -0.5, 1, theta));
	draw_plane(device, 0, 1, 2, 3);
	draw_plane(device, 7, 6, 5, 4);
	draw_plane(device, 0, 4, 5, 1);
	draw_plane(device, 1, 5, 6, 2);
	draw_plane(device, 2, 6, 7, 3);
	draw_plane(device, 3, 7, 4, 0);
}

int main()
{
    fbrender::RenderDevice* device = new fbrender::FBRenderDevice("/dev/fb0");
    device->set_camera({4, 0, 0, 1}, {0, 0, 0, 1}, {0, 0, 1, 1});
    device->enable(RenderDevice::DS_COLOR);
    device->enable(RenderDevice::DS_LIGHTING);
    device->set_light_pos({100, -300, 500, 1});
    device->set_light_diffuse({0.5, 0.5, 0.5});
    device->set_light_ambient({0.5, 0.5, 0.5});
    device->set_material_diffuse({0.3, 0.3, 0.3});
    device->clear_color({0.2, 0.2, 0.3});

    struct timespec ts;
    ts.tv_sec = 100 / 1000;
    ts.tv_nsec = (100 % 1000) * 1000000;

    for (int i = 0; i < 10000; i++) {
        device->clear();
        i = 120;
        draw_box(device, i/100.0);
        device->swap_buffers();
        //nanosleep(&ts, NULL);
    }
    return 0;
}

