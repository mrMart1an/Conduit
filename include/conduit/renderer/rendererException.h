#ifndef CNDT_RENDERER_EXCEPTION_H
#define CNDT_RENDERER_EXCEPTION_H

#include "conduit/exception.h"

#include "conduit/renderer/renderer.h"

namespace cndt {

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

class UnsupportedBackend : public RendererException {
public:
    UnsupportedBackend(std::string_view message, RendererBackend backend) : 
        RendererException(message, backend)
    { }
    UnsupportedBackend(RendererBackend backend) : 
        RendererException("Unsupported renderer backend", backend) 
    { }
};

} // namespace cndt


#endif
