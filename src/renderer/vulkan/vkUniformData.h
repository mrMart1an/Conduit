#ifndef CNDT_VK_UNIFORM_DATA_H
#define CNDT_VK_UNIFORM_DATA_H

#include <glm/glm.hpp>

namespace cndt::vulkan {

// Camera and model matrix data
struct CameraModel {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;    
};

} // namespace cndt::vulkan

#endif
