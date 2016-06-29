#ifndef _FB_RENDER_DEVICE_H_
#define _FB_RENDER_DEVICE_H_

#include "render/render_device.h"

namespace fbrender {

    class FBRenderDevice : public RenderDevice {
    public:
        FBRenderDevice(const char* filename);
        ~FBRenderDevice();

    private:
        int fptr;
        char* fbp;
        long screensize;
    };
}

#endif

