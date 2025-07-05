#ifndef CNDT_VERTEX_H
#define CNDT_VERTEX_H

#include "conduit/renderer/vertexLayout.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace cndt {

// Conduit vertex struct
struct Vertex3D {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 text_coord;

    // Return the vertex layout
    static VertexLayout layout() {
        VertexLayout vertex_layout;

        vertex_layout.setLayout
            <Vertex3D, glm::vec3, glm::vec3, glm::vec2>
        (
            {0, &Vertex3D::position, VertexLayout::Format::f32, 3},
            {1, &Vertex3D::normal, VertexLayout::Format::f32, 3},
            {2, &Vertex3D::text_coord, VertexLayout::Format::f32, 2}
        );

        return vertex_layout;
    }
};
    
} // namespace cndt


#endif
