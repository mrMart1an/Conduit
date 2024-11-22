#ifndef CNDT_GL_EXCEPTIONS
#define CNDT_GL_EXCEPTIONS

#include "conduit/renderer/rendererException.h"

#include <fmt/base.h>

namespace cndt::gl {

/*
 *
 *      OpenGL renderer exception type
 *
 * */

// Renderer generic exception
class OpenGLException : public RendererException {
public:
    template<typename... Args>
    OpenGLException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        RendererException(
            RendererBackend::Vulkan,
            msg, std::forward<Args>(args)...
        )
    { }
};

// Unexpected vulkan error
class UnexpectedGlError : public OpenGLException {
public:
    template<typename... Args>
    UnexpectedGlError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        OpenGLException(msg, std::forward<Args>(args)...)
    { }
};

/*
 *
 *      Shader module exception
 *
 * */

// Generic shader module exception
class ShaderModuleException : public OpenGLException {
public:
    template<typename... Args>
    ShaderModuleException(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        OpenGLException(msg, std::forward<Args>(args)...)
    { }
};

// OpenGL shader module file access error
class ShaderModuleFileError : public ShaderModuleException {
public:
    template<typename... Args>
    ShaderModuleFileError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        ShaderModuleException(msg, std::forward<Args>(args)...)
    { }
};

// OpenGL shader module creation error
class ShaderModuleCreateError : public ShaderModuleException {
public:
    template<typename... Args>
    ShaderModuleCreateError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : 
        ShaderModuleException(msg, std::forward<Args>(args)...)
    { }
};

} // namespace cndt::gl

#endif
