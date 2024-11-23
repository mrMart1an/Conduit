#ifndef CNDT_MESH_ASSET_H
#define CNDT_MESH_ASSET_H

#include "conduit/renderer/vertex.h"

#include "conduit/assets/assetInfo.h"

#include <vector>

namespace cndt {

// Store a conduit mesh asset
class Mesh {
public:
    Mesh() = default;
    ~Mesh() = default;

public:
    // Return a constant reference to the vertices vector
    const std::vector<Vertex3D> getVertexVector() const
    { return m_verticies; }
    // Return a constant reference to the indices vector
    const std::vector<u32> getIndexVector() const
    { return m_indices; }

    // Return a constant pointer to the raw vertex data
    // Store the number of vertices in the given usize variable 
    const Vertex3D* getVertexData(usize& vertex_count_ref) const 
    { 
        vertex_count_ref = m_verticies.size();
        return m_verticies.data(); 
    }
    // Return a constant pointer to the raw index data
    // Store the number of indices in the given usize variable 
    const u32* getIndexData(usize& index_count_ref) const 
    { 
        index_count_ref = m_indices.size();
        return m_indices.data(); 
    }

private:
    // List of vertices of the mesh
    std::vector<Vertex3D> m_verticies;
    // List of indices 
    std::vector<u32> m_indices;
};

// Store a texture asset information
template<>
class AssetInfo<Mesh> : public AssetInfoBase {
public:
    enum class FileType {
        Undefined,
        Obj,
        Gltf
    };

public:
    AssetInfo() = default;
    AssetInfo(
        std::string_view asset_name,
        
        std::filesystem::path mesh_path,
        FileType file_type
    ) : 
        AssetInfoBase(asset_name),
        m_mesh_path(mesh_path),
        m_file_type(file_type)
    { }

    // Get the mesh file path
    std::filesystem::path path() const { return m_mesh_path; }
    
    // Get the mesh file type
    FileType fileType() const { return m_file_type; }
    
private:
    std::filesystem::path m_mesh_path;
    
    FileType m_file_type;
};

} // namespace cndt

#endif 
