#ifndef CNDT_RENDERER_H
#define CNDT_RENDERER_H

#include "conduit/config/engineConfig.h"
#include "conduit/defines.h"

#include "conduit/renderer/ResourceRef.h"
#include "conduit/renderer/graph/graph.h"
#include "conduit/renderer/shader/programBuilder.h"
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

    // Set renderer v-sync 
    virtual void setVsync(bool v_sync) = 0;

    // Toggle renderer v-sync
    virtual void toggleVsync() = 0;
    
    // Get a shader program builder
    virtual RenderRef<ShaderProgramBuilder> getShaderProgramBuilder() = 0;

    // Return a clear render graph ready to be built 
    virtual RenderGraph getRenderGraph() = 0;

    // Execute the given render graph
    virtual void executeGraph(RenderGraph& graph) = 0;
    
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
