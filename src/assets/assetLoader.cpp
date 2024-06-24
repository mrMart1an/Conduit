#include "conduit/assets/assetsManagerException.h"
#include "conduit/internal/assets/assetLocator.h"

#include "conduit/internal/assets/assetLocation.h"
#include "conduit/logging.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <optional>
#include <unordered_map>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace cndt::internal {

// Default path of the builtin table
constexpr const char* builtin_table_path = "resources/builtin_table.json";

// Load the table from a json file
AssetLocator::Table::Table(std::filesystem::path table_path)
{
    if(!std::filesystem::exists(table_path)) {
        throw AssetTableNotFound(std::format(
            "Asset table at: {} not found",
            table_path.string()
        ));
    }
    
    auto type_to_string = [](Type asset_type) {
        switch (asset_type) {
            case Type::Texture: 
                return "Texture";
            case Type::Mesh: 
                return "Mesh";
            case Type::Shader: 
                return "Shader";
                
            default:
                return "Unknow";
        }
    };

    // Add the assets to the map using this lambda
    auto add_assets_to_map = [&](json &elements, Type asset_type) {
        for (auto& element : elements.items()) {
            // Create the map key
            Key key = {asset_type, element.key()};

            // Check if the file exist and create the asset location struct
            std::filesystem::path path(element.value().get<std::string>());
            if(!std::filesystem::exists(path)) {
                throw std::runtime_error(std::format(
                    "Asset file \"{}\" for \"{}\" not found",
                    path.string(),
                    element.key()
                ));
            }

            AssetLocation location(path);

            // Add the element to the table map
            m_table_map[key] = location;

            // Load the asset information
            log::core::debug(
                "asset table: {}; using {} asset \"{}\" -> \"{}\"", 
                table_path.string(),
                type_to_string(asset_type),
                element.key(),
                path.string()
            );
        } 
    };
    
    try {
        // Read the json file
        std::ifstream f(table_path);
        json table = json::parse(f);

        json shaders = table.at("shaders");
        json textures = table.at("textures");
        json meshes = table.at("meshes");

        // Add all the assets
        add_assets_to_map(shaders, Type::Shader);
        add_assets_to_map(textures, Type::Texture);
        add_assets_to_map(meshes, Type::Mesh);

    } catch (std::exception &e) {
        throw AssetTableParseError(std::format(
            "Asset table ({}) parse error: {}",
            table_path.string(),
            e.what()
        ));
    }
}

// Get the asset location in the map
// Return an empty option if the asset doesn't exist
std::optional<AssetLocation> AssetLocator::Table::getLocation(
    Type asset_type,
    std::string_view asset_name
) {
    Key key = {asset_type, asset_name};
    
    if(m_table_map.find(key) != m_table_map.end()) {
        return m_table_map[key];
    }
    
    return std::nullopt;
}

// Create the asset allocator using only the builtin asset table
AssetLocator::AssetLocator() 
{
    m_builtin_table = Table(builtin_table_path);
}

// Create the asset allocator using 
// the builtin asset table and a user defined asset locator
AssetLocator::AssetLocator(std::filesystem::path asset_table_pat)
{
    
}

// Find an asset check if it exist and return a valid asset location struct
// Take asset_name or file path as input
// If the asset is not found use the first builtin of the same type
AssetLocation AssetLocator::locate(
    Type asset_type,
    std::string_view asset_name
) {
    
}

} // namespace cndt::internal
