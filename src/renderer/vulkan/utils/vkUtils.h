#ifndef CNDT_VK_UTILS
#define CNDT_VK_UTILS

#include "renderer/vulkan/utils/vkExceptions.h"

#include <source_location>

#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include <fmt/format.h>

namespace cndt::vulkan {

// Return the string representation of the error code stored in VkResult
static std::string vk_error_str(
    VkResult result,
    std::source_location src_loc = std::source_location::current()
) {
    return fmt::format(
        "({}) in {} at line: {}",
        string_VkResult(result),
        src_loc.file_name(),
        src_loc.line()
    );
}

// Abort the application if the vulkan result is not success
constexpr void vk_check(
    VkResult result,
    std::source_location src_loc = std::source_location::current()
) {
    if (result != VK_SUCCESS) {
        throw UnexpectedVkError(
            "Unexpected vulkan error {}",
            vk_error_str(result, src_loc)
        );
    }
}

} // namespace cndt::vulkan

#endif
