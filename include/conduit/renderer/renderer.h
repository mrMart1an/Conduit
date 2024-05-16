#ifndef CNDT_RENDERER_H
#define CNDT_RENDERER_H

#include "conduit/defines.h"

#include "conduit/window/window.h"

#include <memory>
#include <string_view>

namespace cndt {

// Renderer backend enum
enum class RendererBackend {
    None = 0,
    OpenGl,
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

/*
 *
 *      Renderer exception type
 *
 * */

// Renderer generic exception
class RendererException : public Exception {
public:
    RendererException(std::string_view message, RendererBackend backend) 
        : Exception(message), m_backend(backend) { }
    RendererException(RendererBackend backend) 
        : Exception("Renderer exception"), m_backend(backend) { }
    RendererException() 
        : Exception("Renderer exception"), m_backend(RendererBackend::None) { }

    // Return the backend type
    RendererBackend backend() const { return m_backend; };
    
    // Return the backend type
    const char* backend_str() const { 
        switch (m_backend) {
            case RendererBackend::None: 
                return "None";
            case RendererBackend::OpenGl: 
                return "OpenGl";
            case RendererBackend::Vulkan: 
                return "Vulkan";
        }
    };
    
protected:
    RendererBackend m_backend;
};

} // namespace cndt

#endif
