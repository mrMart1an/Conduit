#ifndef CNDT_MESH_ASSET_H
#define CNDT_MESH_ASSET_H

#include "conduit/renderer/vertex.h"

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

} // namespace cndt

#endif 
