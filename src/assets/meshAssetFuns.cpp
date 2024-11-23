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
    // Get and check json elements
    json src_j = element["src"];
    if (src_j.is_null()) {
        throw AssetTableParseError(
            "Asset mesh \"{}\" missing keyword \"src\"",
            name
        );
    }
    json type_j = element["type"];
    if (type_j.is_null()) {
        throw AssetTableParseError(
            "Asset mesh \"{}\" missing keyword \"type\"",
            name
        );
    }

    std::filesystem::path src = src_j.get<std::filesystem::path>();
    
    // Get the shader type
    std::string_view type_str = type_j.get<std::string_view>();
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
