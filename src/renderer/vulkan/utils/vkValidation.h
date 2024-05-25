#ifndef CNDT_VK_VALIDATION_H
#define CNDT_VK_VALIDATION_H

#include <array>
#include <vector>

#ifndef NDEBUG 

#define VK_VALIDATION
#define VK_DEBUG_MESSENGER

#endif

namespace cndt::vulkan {

// Store the validation layer for the vulkan implementation
class Validation {
private:
    Validation() = delete;

    // Store a list of validation layer to use
    #ifdef VK_VALIDATION
    static constexpr std::array<const char*, 1> layers_str = 
    {
        "VK_LAYER_KHRONOS_validation", 
    };
    #else
    static constexpr std::array<const char*, 0> layers_str = {};
    #endif

public:
    // Return a vector containing the validation layer
    // this function also check if the validation are supported
    // by the implementation 
    static const std::vector<const char*> getLayers();
};

} // namespace cndt::vulkan

#endif
