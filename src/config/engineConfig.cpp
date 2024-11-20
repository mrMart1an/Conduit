#include "conduit/config/engineConfig.h"
#include "conduit/config/configException.h"
#include "nlohmann/json_fwd.hpp"

#include <nlohmann/json.hpp>

#include <fstream>
#include <optional>
#include <string>

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

    // Asset manager settings
    assets.user_table_path = std::nullopt;
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

    // Assets manager settings
    if (config.assets.user_table_path.has_value())
        assets.user_table_path = config.assets.user_table_path;
}

// Parse a json filed from the settings
template <typename Type>
std::optional<Type> parseJsonField(
    nlohmann::json field,
    std::string key
) {
    nlohmann::json element = field[key];

    if (!element.is_null()) {
        return element.get<Type>();
    } else {
        return std::nullopt;
    }
}

// Parse a json configuration file
void EngineConfig::parseJson(std::filesystem::path path)
{
    // Check if the file exist
    if(!std::filesystem::exists(path)) {
        throw ConfigFileNotFound(
            "Config file at: \"{}\" not found",
            path.string()
        );
    }

    try {
        // Parse the json file
        std::ifstream f(path);
        nlohmann::json config_data = nlohmann::json::parse(f);

        // Parse window config
        nlohmann::json window_data = config_data["window"];

        window.width = parseJsonField<u32>(window_data, "width"); 
        window.height = parseJsonField<u32>(window_data, "height"); 

        window.floating = parseJsonField<bool>(
            window_data, "floating"
        );
        window.resizable = parseJsonField<bool>(
            window_data, "resizable"
        );
        window.start_fullscreen = parseJsonField<bool>(
            window_data, "start_fullscreen"
        );

        // Parse renderer config
        nlohmann::json renderer_data = config_data["renderer"];

        nlohmann::json backend = renderer_data["backend"];

        if (!backend.is_null()) {
            std::string backend_str = backend.get<std::string>();

            if (backend_str == "vulkan") {
                renderer.backend = RendererBackend::Vulkan;
            } else if (backend_str == "opengl") {
                renderer.backend = RendererBackend::OpenGL;

            } else {
                throw ConfigParseError(
                    "Json renderer backend settings invalid ({}) - {}",
                    backend_str,
                    "available option \"opengl\", \"vulkan\""
                );
            }

        } else {
            renderer.backend = std::nullopt;
        }

        // Parse assets config
        nlohmann::json asset_data = config_data["asset"];

        assets.user_table_path = parseJsonField<std::filesystem::path>(
            asset_data, "user_table_path"
        );        

    } catch (std::exception &e) {
        throw ConfigParseError(
            "Config file ({}) parse error: {}",
            path.string(),
            e.what()
        );
    }
}

} // namespace cndt

