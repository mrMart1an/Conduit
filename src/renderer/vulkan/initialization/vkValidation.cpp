#include "conduit/defines.h"
#include "conduit/logging.h"

#include "renderer/vulkan/vkUtils.h"
#include "vkValidation.h"

#include <vector>

#include <vulkan/vulkan.h>

namespace cndt::vulkan {

// Return a vector containing the validation layer
// this function also check if the validation are supported
// by the implementation 
const std::vector<const char*> Validation::getLayers() 
{
    std::vector<const char*> out_layers;
    out_layers.reserve(Validation::layers_str.size());

    // Fetch the layers properties
    u32 prop_count;
    vk_check(vkEnumerateInstanceLayerProperties(&prop_count, VK_NULL_HANDLE));

    std::vector<VkLayerProperties> layer_properties(prop_count);
    vk_check(vkEnumerateInstanceLayerProperties(
        &prop_count, layer_properties.data()
    ));

    // Check for validation layer support
    for (usize i = 0; i < layers_str.size(); i++) {
        bool layer_found = false;
    
        for (u32 j = 0; j < prop_count; j++) {
            // Get the required and the available event name 
            const std::string_view required_layer = layers_str[i];
            const std::string_view available_layer = 
                layer_properties[j].layerName;

            if (required_layer.compare(available_layer) == 0) {
                layer_found = true;
                break;
            }
        }

        // If the validation layer is available append it to the output vector
        if (layer_found) {
            out_layers.push_back(layers_str[i]);
            
        } else {
            // If no compatible layer was found log a warning
            log::core::warn(
                "Vulkan validation layer not found: {}",
                layers_str[i]
            );
        }
    }

    return out_layers;
}

} // namespace cndt::vulkan 
