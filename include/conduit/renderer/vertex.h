#ifndef CNDT_VERTEX_H
#define CNDT_VERTEX_H

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace cndt {

// Conduit vertex struct
struct Vertex3D {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 text_coord;
};
    
} // namespace cndt


#endif
