#include "conduit/assets/assetsManagerException.h"
#include "conduit/assets/mesh.h"
#include "conduit/assets/texture.h"
#include "conduit/internal/assets/assetStorage.h"
#include "conduit/assets/assetInfo.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

namespace cndt::internal {

// Parse shader from the given shader info 
std::shared_ptr<AssetStorage<Shader>> loadShader(AssetInfo<Shader>& info) 
{
    // Load the spv code byte in memory 
    std::filesystem::path spv_path = info.pathSpv();
    std::vector<u32> spv_code;

    try {
        std::ifstream spv_file(
            spv_path,
            std::ifstream::ate | std::ifstream::binary
        );

        if(spv_file.fail()) {
            throw ShaderLoadingError(
                "failed to open file: {}",
                spv_path.string()
            );
        }

        // Get the code size and go to the beginning of the file
        usize spv_size = spv_file.tellg() / 4;
        spv_file.seekg(0);

        // Load the code in the buffer
        spv_code.resize(spv_size);
        spv_file.read((char*)spv_code.data(), spv_size * 4);

        spv_file.close();

    } catch (std::exception& e) {
        throw ShaderLoadingError(
            "Shader spv code loading error: {}",
            e.what()
        );
    }

    // Create and return a shared pointer storing the asset storage
    return std::make_shared<AssetStorage<Shader>>(
        info,
        std::make_unique<Shader>(
            spv_code,
            info.shaderType()
        )
    );
}

// Parse texture from the given texture info 
std::shared_ptr<AssetStorage<Texture>> loadTexture(AssetInfo<Texture>& info) 
{
    return std::make_shared<AssetStorage<Texture>>(
        info,
        std::make_unique<Texture>()
    );
}

// Load a mesh from the given mesh info 
std::shared_ptr<AssetStorage<Mesh>> loadMesh(AssetInfo<Mesh>& info)
{
    return std::make_shared<AssetStorage<Mesh>>(
        info,
        std::make_unique<Mesh>()
    );
}

} // namespace cndt::internal
