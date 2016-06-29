#include "render/fb_render_device.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

namespace fbrender {

    FBRenderDevice::FBRenderDevice(const char* filename)
    {
        fbp = nullptr;
        int fd = open(filename, O_RDWR);

        struct fb_fix_screeninfo finfo;
        struct fb_var_screeninfo vinfo;

        if (fd < 0) return;
        if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo)) return;
        if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)) return;

        screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
        char* framebuffer = (char *) mmap (0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 

        if (framebuffer == MAP_FAILED) {
            close(fd);
            return;
        }

        fptr = fd;
        fbp = framebuffer;
        init(vinfo.xres, vinfo.yres, framebuffer);
    }

    FBRenderDevice::~FBRenderDevice()
    {
        if (fbp) {
            munmap(fbp, screensize);
        }
        if (fptr > 0) close(fptr);
    }
}

