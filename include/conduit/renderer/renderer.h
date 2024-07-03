#ifndef CNDT_RENDERER_H
#define CNDT_RENDERER_H

#include "conduit/defines.h"

#include "conduit/window/window.h"

#include <memory>

namespace cndt {

// Renderer backend enum
enum class RendererBackend {
    None = 0,
    OpenGL,
    Vulkan
};

class Application;

// Conduit renderer interface
class Renderer {
    friend class Application;

protected:
    // Render initialization configuration
    struct Config {
        Config(u32 width, u32 height)
        : width(width), height(height) { };
        
        u32 width, height;
    };
 
public:
    Renderer() = default;
    virtual ~Renderer() = default;

    // Draw a frame and present it
    virtual void draw() = 0;

    // Set renderer v-sync 
    virtual void setVsync(bool v_sync) = 0;

    // Toggle renderer v-sync
    virtual void toggleVsync() = 0;
    
protected:
    // Initialize the renderer implementation
    virtual void initialize(
        const char *app_title,
        Window *window_p
    ) = 0;
    
    // Shutdown the renderer implementation
    virtual void shutdown() = 0;

    // Resize the renderer viewport
    virtual void resize(u32 width, u32 height) = 0;

    // Get a pointer to an uninitialized renderer object
    // for the given implementation
    static std::unique_ptr<Renderer> getRenderer(RendererBackend backend);
};

} // namespace cndt

#endif
