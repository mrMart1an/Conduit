#include "conduit/assets/assetsManagerException.h"
#include "conduit/assets/assetsTypeFuns.h"
#include "conduit/assets/texture.h"

namespace cndt {

using json = nlohmann::json;

// Return texture asset table name (the json key to the asset table)
template <>
std::string assetTableName<Texture>() {
    return "textures";
}

// Parse texture info from json 
template <>
AssetInfo<Texture> parseTableEntry<Texture>(
    std::string_view name,
    json element
) {
    std::filesystem::path src = 
        element.at("src")
        .get<std::filesystem::path>();

    // Norm will be empty unless explicitly specified in the json element
    std::filesystem::path norm = {};
    
    if (element.find("norm") != element.end()) {
        json norm_j = element.at("norm");

        if (!norm_j.is_null()) {
            norm = norm_j.get<std::filesystem::path>();
        }
    }

    // Test path validity
    if (!std::filesystem::exists(src)) {
        throw AssetTableParseError(
            "Source file \"{}\" for texture asset \"{}\" not found",
            src.string(),
            name
        );
    }
    if (!std::filesystem::exists(norm) && !norm.empty()) {
        throw AssetTableParseError(
            "Normal file \"{}\" for texture asset \"{}\" not found",
            norm.string(),
            name
        );
    }
    
    // Debug log
    log::core::debug(
        "Using texture asset: \"{}\"",
        name
    );
    
    return AssetInfo<Texture>(
        name,

        src,
        norm
    );
}

// Load texture from the given texture info 
template <>
std::unique_ptr<Texture> loadAsset<Texture>(
    AssetInfo<Texture>& info
) {
    throw AssetException("Texture loading not implemented");

    return std::make_unique<Texture>();
}

} // namespace cndt
