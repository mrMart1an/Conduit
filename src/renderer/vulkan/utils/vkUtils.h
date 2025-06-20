#ifndef CNDT_VK_UTILS
#define CNDT_VK_UTILS

#include "conduit/defines.h"

#include "renderer/vulkan/utils/vkExceptions.h"

#include <vulkan/vulkan_core.h>
#include <vulkan/vk_enum_string_helper.h>

#include <fmt/format.h>

namespace cndt::vulkan {

// Return the string representation of the error code stored in VkResult
static std::string vk_error_str(
    VkResult result,
    std::string_view file_name,
    u32 line
) {
    return fmt::format(
        "({}) in {} at line: {}",
        string_VkResult(result),
        file_name,
        line
    );
}

// Abort the application if the vulkan result is not success
constexpr void vk_check(
    VkResult result,
    std::string_view file_name,
    u32 line
) {
    if (result != VK_SUCCESS) {
        throw UnexpectedVkError(
            "Unexpected vulkan error {}",
            vk_error_str(result, file_name, line)
        );
    }
}

#define VK_ERROR_STR(result) \
    cndt::vulkan::vk_error_str(result, __FILE__, __LINE__)

#define VK_CHECK(result) \
    cndt::vulkan::vk_check(result, __FILE__, __LINE__)

} // namespace cndt::vulkan

#endif
