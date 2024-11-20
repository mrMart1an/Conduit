#ifndef CNDT_GL_RENDERER_H
#define CNDT_GL_RENDERER_H

#include "conduit/renderer/renderer.h"
#include "conduit/window/window.h"

namespace cndt::gl {

class GlRenderer : public Renderer {
public:
    GlRenderer() = default;
    ~GlRenderer() override = default;

    // Set renderer v-sync 
    void setVsync(bool v_sync) override;

    // Toggle renderer v-sync
    void toggleVsync() override;
    
    // Get a shader program builder
    RenderRef<ShaderProgramBuilder> getShaderProgramBuilder() override;

    // Return a cleared render packet ready to be built 
    RenderPacket getRenderPacket() override; 

    // Execute the given render packet
    void executePacket(RenderPacket& packet) override;
    
protected:
    // Initialize the renderer implementation
    void initialize(
        EngineConfig::Renderer config,
        const char *app_title,

        Window *window_p
    ) override;
    
    // Shutdown the renderer implementation
    void shutdown() override;

    // Resize the renderer viewport
    void resize(u32 width, u32 height) override;

private:
    // Dimensions of the renderer viewport
    u32 m_frame_width, m_frame_height;
    // Store the current status of v-sync
    bool m_v_sync;

    // Store true and halt rendering if the window is minimized
    bool m_minimized;

    // Count the total number of frame rendered by the renderer
    u64 m_frame_count;

    // Store a pointer to the window that own the GL context
    Window *m_contex_window_p;
};

} // namespace cndt::gl

#endif
