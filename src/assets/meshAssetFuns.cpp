#include "conduit/assets/assetInfo.h"
#include "conduit/assets/assetsManagerException.h"
#include "conduit/assets/assetsTypeFuns.h"
#include "conduit/assets/mesh.h"
#include "conduit/logging.h"

#include <tiny_obj_loader.h>

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
    // Load obj file 
    if (info.fileType() == AssetInfo<Mesh>::FileType::Obj) {
        // Check file path
        std::filesystem::path path = info.path();
    
        if(!std::filesystem::exists(path)) {
            throw AssetLoadingError(
                "Mesh obj file at: \"{}\" not found",
                path.string()
            );
        }

        // Open the file with tiny obj
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        
        std::string warn;
        std::string err;
        
        bool ret = tinyobj::LoadObj(
            &attrib,
            &shapes,
            &materials,
            &warn, &err,
            path.c_str()
        );
        
        // Check for error
        if (!warn.empty()) {
            log::core::warn(
                "Mesh obj file loading: {}",
                warn
            );
        }
        
        if (!err.empty()) {
            throw AssetLoadingError(
                "Mesh obj loading error: {}",
                err
            );
        }
        
        if (!ret) {
            throw AssetLoadingError(
                "Mesh obj loading error: {}",
                "Unknown error"
            );
        } 

        if (shapes.size() == 0) {
            throw AssetLoadingError(
                "Mesh obj loading error: {}",
                "No shape in obj file"
            );
        }

        // Calculate the number of vertices for preallocation
        usize shape_count = shapes.size();

        usize vertices_count = 0;
        for (usize i = 0; i < shape_count; i++) {
            vertices_count += shapes[i].mesh.indices.size();
        }

        // Storage buffer
        std::vector<Vertex3D> vertices;
        vertices.reserve(vertices_count);
        std::vector<u32> indices;
        indices.reserve(vertices_count);

        // Loop over shapes
        // TODO make this more efficient
        u32 index = 0;
        for (usize s = 0; s < shape_count; s++) {
            // Loop over faces(polygon)
            for (usize i = 0; i < shapes[s].mesh.indices.size(); i++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[i];

                Vertex3D vertex = { };
            
                vertex.position.x =
                    attrib.vertices[3*size_t(idx.vertex_index)+0];
                vertex.position.y =
                    attrib.vertices[3*size_t(idx.vertex_index)+1];
                vertex.position.z =
                    attrib.vertices[3*size_t(idx.vertex_index)+2];
            
                // Check if `normal_index` is zero or positive. 
                // Negative = no normal data
                if (idx.normal_index >= 0) {
                    vertex.normal.x =
                        attrib.normals[3*size_t(idx.normal_index)+0];
                    vertex.normal.y =
                        attrib.normals[3*size_t(idx.normal_index)+1];
                    vertex.normal.z =
                        attrib.normals[3*size_t(idx.normal_index)+2];
                }
            
                // Check if `texcoord_index` is zero or positive.
                // Negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    vertex.normal.x =
                        attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    vertex.normal.y =
                        attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                }

                // Push vertex and index to the storage buffer
                vertices.push_back(vertex);
                indices.push_back(index);

                // Increment the index
                index += 1;
            }
        }

        // Return the mesh
        return std::make_unique<Mesh>(vertices, indices);
    }

    // Unimplemented loading function 
    throw AssetLoadingError(
        "Mesh loading function for this file type not implemented"
    );

    return std::make_unique<Mesh>();
}

} // namespace cndt
