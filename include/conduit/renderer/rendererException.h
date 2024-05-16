#ifndef CNDT_RENDERER_EXCEPTION_H
#define CNDT_RENDERER_EXCEPTION_H

#include "conduit/renderer/renderer.h"

namespace cndt {

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
