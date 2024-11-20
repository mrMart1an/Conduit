#include "renderer/opengl/glRenderer.h"

#include <glad/glad.h>

namespace cndt::gl {

// Initialize the opengl renderer
void GlRenderer::initialize(
    EngineConfig::Renderer,
    const char *app_title,

    Window *window_p
) 
{
    m_frame_count = 0;
    m_v_sync = true;

    // Store the viewport dimension
    m_frame_width = window_p->getWindowData().buffer_width;
    m_frame_height = window_p->getWindowData().buffer_height;
    
    m_minimized = !(m_frame_height != 0 && m_frame_width != 0); 

    // Store the window pointer
    m_contex_window_p = window_p;

    // Set v sync 
    setVsync(m_v_sync);

    // Set the view port
    if (!m_minimized)
        glViewport(0, 0, m_frame_width, m_frame_height);
}

// Shutdown the renderer
void GlRenderer::shutdown()
{

}

// Resize the renderer viewport
void GlRenderer::resize(u32 width, u32 height) 
{
    if (width != 0 && height != 0) {
        m_frame_width = width;
        m_frame_height = height;
        
        m_minimized = false;
        
        glViewport(0, 0, m_frame_width, m_frame_height);
    } else {
        m_minimized = true;
    }
}

// Set renderer v-sync 
void GlRenderer::setVsync(bool v_sync)
{
    m_contex_window_p->glSetVSync(v_sync);
    m_v_sync = v_sync;
}

// Toggle renderer v-sync
void GlRenderer::toggleVsync()
{
    m_contex_window_p->glSetVSync(!m_v_sync);
}

RenderRef<ShaderProgramBuilder> GlRenderer::getShaderProgramBuilder() 
{
    return RenderRef<ShaderProgramBuilder>();
}

// Return a clear render packet ready to be built 
RenderPacket GlRenderer::getRenderPacket() 
{
    return RenderPacket();
}

// Execute the given render packet
void GlRenderer::executePacket(RenderPacket& graph)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Present the frame by swapping the buffer
    m_contex_window_p->glSwapBuffer();
}

}

