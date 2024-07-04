#include "conduit/config/engineConfig.h"

namespace cndt {

// Fill all the field with the default values
void EngineConfig::setDefault()
{
    // Window default settings
    window.start_fullscreen = false;
    window.resizable = false;
    window.floating = false;
    window.width = 800;
    window.height = 600;

    // Renderer default settings
    renderer.backend = RendererBackend::Vulkan;
}

// Merge the configuration struct with another one,
// if a field is not nullopt in the given object 
// it overwrite the settings in the current config struct
void EngineConfig::merge(EngineConfig& config)
{
    // Merge window settings
    if (config.window.start_fullscreen.has_value())
        window.start_fullscreen = config.window.start_fullscreen;

    if (config.window.resizable.has_value())
        window.resizable = config.window.resizable;

    if (config.window.floating.has_value())
        window.floating = config.window.floating;

    if (config.window.width.has_value())
        window.width = config.window.width;

    if (config.window.height.has_value())
        window.height = config.window.height;

    // Merge renderer settings
    if (config.renderer.backend.has_value())
        renderer.backend = config.renderer.backend;
}

} // namespace cndt

