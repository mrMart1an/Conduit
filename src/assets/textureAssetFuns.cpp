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

    // Debug log
    log::core::debug(
        "Using texture asset: \"{}\"",
        name
    );
    
    return AssetInfo<Texture>(
        name,
        src
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
