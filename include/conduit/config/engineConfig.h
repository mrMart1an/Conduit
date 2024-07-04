#ifndef CNDT_ENGINE_CONFIG_H
#define CNDT_ENGINE_CONFIG_H

#include "conduit/defines.h"

#include "conduit/renderer/backendEnum.h"

#include <filesystem>
#include <optional>

namespace cndt {

struct EngineConfig {
    // The default constructor initialize all the option to nullopt
    EngineConfig() :
        window(),
        renderer()
    { }

    // Engine window start configuration
    struct Window {
        Window() : 
            start_fullscreen(std::nullopt),
            resizable(std::nullopt),
            floating(std::nullopt),
            width(std::nullopt),
            height(std::nullopt)
        { }

        // Start the window in fullscreen
        std::optional<bool> start_fullscreen;

        // Make the window resizable    
        std::optional<bool> resizable;
        // Make the window float other the other
        std::optional<bool> floating;

        // Window start width, ignored if start fullscreen is set
        std::optional<u32> width;
        // Window start height, ignored if start fullscreen is set
        std::optional<u32> height;
    } window;

    // Conduit renderer start options
    struct Renderer {
        Renderer() : 
            backend(std::nullopt)
        { }

        std::optional<RendererBackend> backend;
    } renderer;

    // Store the asset manager settings
    struct Assets {
        Assets() :
            user_table_path(std::nullopt)
        { }

        std::optional<std::filesystem::path> user_table_path;
    } assets;

    // Fill all the field with the default values
    void setDefault();

    // Merge the configuration struct with another one,
    // if a field is not nullopt in the given object 
    // it overwrite the settings in the current config struct
    void merge(EngineConfig& config);
};

} // namespace cndt

#endif
