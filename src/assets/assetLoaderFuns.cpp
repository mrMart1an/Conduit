#include "conduit/assets/assetsManagerException.h"
#include "conduit/assets/assetInfo.h"
#include "conduit/assets/assetsTypeFuns.h"

#include "conduit/assets/mesh.h"
#include "conduit/assets/shader.h"
#include "conduit/assets/texture.h"

#include "conduit/internal/assets/assetStorage.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <vector>

namespace cndt {

// Parse shader from the given shader info 
template <>
std::shared_ptr<AssetStorage<Shader>> loadAsset<Shader>(
    AssetInfo<Shader>& info
) {
    // Load the vulkan spv code byte in memory 
    std::filesystem::path vk_spv_path = info.pathVkSpv();
    std::vector<u32> vk_spv_code;

    try {
        std::ifstream vk_spv_file(
            vk_spv_path,
            std::ifstream::ate | std::ifstream::binary
        );

        if(vk_spv_file.fail()) {
            throw ShaderLoadingError(
                "failed to open file: {}",
                vk_spv_path.string()
            );
        }

        // Get the code size and go to the beginning of the file
        usize spv_size = vk_spv_file.tellg() / sizeof(u32);
        vk_spv_file.seekg(0);

        // Load the code in the buffer
        vk_spv_code.resize(spv_size);
        vk_spv_file.read((char*)vk_spv_code.data(), spv_size * sizeof(u32));

        vk_spv_file.close();

    } catch (std::exception& e) {
        throw ShaderLoadingError(
            "Shader spv code loading error: {}",
            e.what()
        );
    }

    // Load the vulkan glsl code byte in memory 
    std::filesystem::path vk_glsl_path = info.pathVkGlsl();
    std::vector<char> vk_glsl_code;

    try {
        std::ifstream vk_glsl_file(
            vk_glsl_path,
            std::ifstream::ate | std::ifstream::binary
        );

        if(vk_glsl_file.fail()) {
            throw ShaderLoadingError(
                "failed to open file: {}",
                vk_spv_path.string()
            );
        }

        // Get the code size and go to the beginning of the file
        usize glsl_size = vk_glsl_file.tellg();
        vk_glsl_file.seekg(0);

        // Load the code in the buffer
        vk_glsl_code.resize(glsl_size);
        vk_glsl_file.read((char*)vk_glsl_code.data(), glsl_size);

        vk_glsl_file.close();

    } catch (std::exception& e) {
        throw ShaderLoadingError(
            "Shader vulkan glsl code loading error: {}",
            e.what()
        );
    }

    // Load the vulkan glsl code byte in memory 
    std::filesystem::path gl_glsl_path = info.pathVkGlsl();
    std::vector<char> gl_glsl_code;

    try {
        std::ifstream gl_glsl_file(
            gl_glsl_path,
            std::ifstream::ate | std::ifstream::binary
        );

        if(gl_glsl_file.fail()) {
            throw ShaderLoadingError(
                "failed to open file: {}",
                vk_spv_path.string()
            );
        }

        // Get the code size and go to the beginning of the file
        usize glsl_size = gl_glsl_file.tellg();
        gl_glsl_file.seekg(0);

        // Load the code in the buffer
        gl_glsl_code.resize(glsl_size);
        gl_glsl_file.read((char*)gl_glsl_code.data(), glsl_size);

        gl_glsl_file.close();

    } catch (std::exception& e) {
        throw ShaderLoadingError(
            "Shader OpenGL glsl code loading error: {}",
            e.what()
        );
    }

    // Make the asset available
    info.setAvailable(true);
    info.incrementVersion();

    // Create and return a shared pointer storing the asset storage
    return std::make_shared<AssetStorage<Shader>>(
        info,
        std::make_unique<Shader>(
            vk_spv_code,
            vk_glsl_code,

            gl_glsl_code,

            info.shaderType()
        )
    );
}

// Load a mesh from the given mesh info 
template <>
std::shared_ptr<AssetStorage<Mesh>> loadAsset<Mesh>(
    AssetInfo<Mesh>& info
) {
    return std::make_shared<AssetStorage<Mesh>>(
        info,
        std::make_unique<Mesh>()
    );
}

// Parse texture from the given texture info 
template <>
std::shared_ptr<AssetStorage<Texture>> loadAsset<Texture>(
    AssetInfo<Texture>& info
) {
    return std::make_shared<AssetStorage<Texture>>(
        info,
        std::make_unique<Texture>()
    );
}

} // namespace cndt::internal
