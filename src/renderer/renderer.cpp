#include "conduit/renderer/renderer.h"
#include "conduit/renderer/rendererException.h"

#include <memory>

#include "buildConfig.h"

#ifdef CNDT_VULKAN_BACKEND
#include "renderer/vulkan/vkRenderer.h"
#endif

namespace cndt {

// Get a pointer to an uninitialized renderer object
// for the given implementation
std::unique_ptr<Renderer> Renderer::getRenderer(RendererBackend backend)
{
    if (backend == RendererBackend::Vulkan) {
        #ifdef CNDT_VULKAN_BACKEND
        
        return std::make_unique<vulkan::VkRenderer>();
        
        #else
        throw UnsupportedBackend(
            "getRenderer: vulkan backend is disable", backend
        );
        #endif

    } else if (backend == RendererBackend::OpenGl) {
        throw UnsupportedBackend(
            "getRenderer: OpenGl is currently not supported", backend
        );
        
    } else {
        throw UnsupportedBackend("getRenderer: unsupported backend", backend);
    } 
}

} // namespace cndt
