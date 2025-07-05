#include "conduit/renderer/backendEnum.h"
#include "conduit/renderer/renderer.h"
#include "conduit/renderer/rendererException.h"

#include <memory>

#include "buildConfig.h"

#ifdef CNDT_VULKAN_BACKEND
#include "renderer/vulkan/vkRenderer.h"
#endif
#ifdef CNDT_OPENGL_BACKEND
#include "renderer/opengl/glRenderer.h"
#endif

namespace cndt {

// Get a pointer to an uninitialized renderer object
// for the given implementation
std::unique_ptr<Renderer> Renderer::getRenderer(
    EngineConfig::Renderer config
) {
    RendererBackend backend = config.backend.value_or(RendererBackend::None);

    if (backend == RendererBackend::Vulkan) {
        #ifdef CNDT_VULKAN_BACKEND
        
        return std::make_unique<vulkan::VkRenderer>();
        
        #else
        throw UnsupportedBackend(
            "getRenderer: vulkan backend is disable", backend
        );
        #endif

    } else if (backend == RendererBackend::OpenGL) {
        #ifdef CNDT_OPENGL_BACKEND
        
        return std::make_unique<gl::GlRenderer>();
        
        #else
        throw UnsupportedBackend(
            "getRenderer: vulkan backend is disable", backend
        );
        #endif

    } else {
        throw UnsupportedBackend(backend, "getRenderer: unsupported backend");
    } 
}

} // namespace cndt
