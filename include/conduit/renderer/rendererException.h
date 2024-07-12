#ifndef CNDT_RENDERER_EXCEPTION_H
#define CNDT_RENDERER_EXCEPTION_H

#include "conduit/exception.h"

#include "conduit/renderer/backendEnum.h"

#include <fmt/base.h>
#include <utility>

namespace cndt {

/*
 *
 *      Renderer exception type
 *
 * */

// Renderer generic exception
class RendererException : public Exception {
public:
    template<typename... Args>
    RendererException(
        RendererBackend backend,
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        Exception(msg, std::forward<Args>(args)...),
        m_backend(backend) 
    { }

    // Return the backend type
    RendererBackend backend() const { return m_backend; };
    
    // Return the backend type
    const char* backend_str() const { 
        switch (m_backend) {
            case RendererBackend::None: 
                return "None";
            case RendererBackend::OpenGL: 
                return "OpenGl";
            case RendererBackend::Vulkan: 
                return "Vulkan";
        }
    };
    
protected:
    RendererBackend m_backend;
};

// Unsupported backend exception
class UnsupportedBackend : public RendererException {
public:
    template<typename... Args>
    UnsupportedBackend(
        RendererBackend backend,
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        RendererException(
            backend,
            msg, std::forward<Args>(args)...
        )
    { }
};

// Shader program invalid option
class ShaderProgramInvalidOption : public RendererException {
public:
    template<typename... Args>
    ShaderProgramInvalidOption(
        RendererBackend backend,
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        RendererException(
            backend,
            msg, std::forward<Args>(args)...
        )
    { }
};


// Shader program invalid build
class InvalidShaderProgram : public RendererException {
public:
    template<typename... Args>
    InvalidShaderProgram(
        RendererBackend backend,
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        RendererException(
            backend,
            msg, std::forward<Args>(args)...
        )
    { }
};

} // namespace cndt

#endif
