#ifndef CNDT_RENDERER_H
#define CNDT_RENDERER_H

#include "conduit/defines.h"
namespace cndt {

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
 
protected:
    // Initialize the renderer implementation
    virtual void intialize() = 0;
    
    // Shutdown the renderer implementation
    virtual void shutdown() = 0;

    // Resize the renderer viewport
    virtual void resize(u32 width, u32 height) = 0;
};

} // namespace cndt

#endif
