#include "conduit/assets/assetsManagerException.h"
#include "conduit/assets/assetsTypeFuns.h"
#include "conduit/assets/mesh.h"

namespace cndt {

using json = nlohmann::json;

// Return mesh asset table name (the json key to the asset table)
template <>
std::string assetTableName<Mesh>() {
    return "meshes";
}

// Parse mesh info from json 
template <>
AssetInfo<Mesh> parseTableEntry<Mesh>(
    std::string_view name,
    json element
) {   
    std::filesystem::path src = 
        element.at("src")
        .get<std::filesystem::path>();
    
    // Debug log
    log::core::debug(
        "Using mesh asset: \"{}\"",
        name
    );
    
    // Get the shader type
    std::string_view type_str = element.at("type").get<std::string_view>();
    AssetInfo<Mesh>::FileType type;
    
    if (type_str == "obj")
        type = AssetInfo<Mesh>::FileType::Obj;
        
    else {
        throw AssetTableParseError(
            "Asset mesh \"{}\" unknow file type: \"{}\"",
            name,
            type_str
        );
    }
    
    return AssetInfo<Mesh>(
        name,
        src,
        type
    );
}

// Load a mesh from the given mesh info 
template <>
std::unique_ptr<Mesh> loadAsset<Mesh>(
    AssetInfo<Mesh>& info
) {
    throw AssetException("Mesh loading not implemented");

    return std::make_unique<Mesh>();
}

} // namespace cndt
