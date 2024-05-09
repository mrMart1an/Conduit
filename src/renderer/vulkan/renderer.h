#ifndef CNDT_VK_RENDERER_H
#define CNDT_VK_RENDERER_H

#include "conduit/renderer/renderer.h"

namespace cndt {

class VkRenderer : public Renderer {
    
private:
    // Initialize the renderer implementation
    void intialize();
    
    // Shutdown the renderer implementation
    void shutdown();

    // Resize the renderer viewport
    void resize(u32 width, u32 height);
};

}

#endif
