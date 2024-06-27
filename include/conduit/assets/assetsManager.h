#ifndef CNDT_ASSETS_MANAGER_H
#define CNDT_ASSETS_MANAGER_H

#include "conduit/assets/handle.h"
#include "conduit/assets/shader.h"

#include "conduit/internal/assets/assetsCache.h"

#include <string_view>

namespace cndt {

// Manage and cache the engine asset
class AssetsManager {
public:
    // Create an asset manager using only the builtin asset table
    AssetsManager();
    // Create an asset manager using 
    // the builtin asset table and a user defined asset table
    AssetsManager(std::filesystem::path asset_table_path);
    
    // Get an asset handle from the given asset name or file path
    template<typename AssetType>
    Handle<AssetType> get(std::string_view asset_name);

private:
    internal::AssetsCache<Shader> m_shader_cache;
};

} // namespace cndt

#endif
