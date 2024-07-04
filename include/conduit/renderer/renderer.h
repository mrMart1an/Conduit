#ifndef CNDT_RENDERER_H
#define CNDT_RENDERER_H

#include "conduit/config/engineConfig.h"
#include "conduit/defines.h"

#include "conduit/window/window.h"

#include <memory>

namespace cndt {

class Application;

// Conduit renderer interface
class Renderer {
    friend class Application;

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
        EngineConfig::Renderer config,
        const char *app_title,

        Window *window_p
    ) = 0;
    
    // Shutdown the renderer implementation
    virtual void shutdown() = 0;

    // Resize the renderer viewport
    virtual void resize(u32 width, u32 height) = 0;

    // Get a pointer to an uninitialized renderer object
    // for the given implementation
    static std::unique_ptr<Renderer> getRenderer(
        EngineConfig::Renderer config
    );
};

} // namespace cndt

#endif
