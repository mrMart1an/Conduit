#ifndef CNDT_VK_UTILS
#define CNDT_VK_UTILS

#include "renderer/vulkan/vkExceptions.h"

#include <format>
#include <source_location>

#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

namespace cndt::vulkan {

// Return the string representation of the error code stored in VkResult
static const char* vk_error_str(VkResult result)
{
return string_VkResult(result);
}

// Abort the application if the vulkan result is not success
constexpr void vk_check(
    VkResult result,
    std::source_location src_loc = std::source_location::current()
) {
    if (result != VK_SUCCESS) {
        throw UnexpectedError(
            std::format(
                "Unexpected vulkan error ({}) in {} at line: {}",
                vk_error_str(result),
                src_loc.file_name(),
                src_loc.line()
            )
        );
    }
}


} // namespace cndt::vulkan

#endif