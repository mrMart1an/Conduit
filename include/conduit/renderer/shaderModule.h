#ifndef CNDT_RENDERER_SHADER_MODULE_H
#define CNDT_RENDERER_SHADER_MODULE_H

#include "conduit/renderer/backendEnum.h"

namespace cndt {

// Backend independent shader module abstraction 
class ShaderModule {
public:
    // Shader module type
    enum class Type {
        Undefined,
        Vertex,
        Fragment,
        Geometry,
        TessellationControl,
        TessellationEval,
        Compute
    };

public:

    // Return the renderer backend that own this resource
    virtual RendererBackend backend() const = 0;
};

} // namespace cndt

#endif
