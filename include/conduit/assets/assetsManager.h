#ifndef CNDT_ASSETS_MANAGER_H
#define CNDT_ASSETS_MANAGER_H

#include "conduit/assets/handle.h"
#include "conduit/assets/shader.h"

#include "conduit/internal/assets/assetParser.h"
#include "conduit/internal/assets/assetsCache.h"

#include <string_view>
#include <vector>

namespace cndt {

// Manage and cache the engine asset
class AssetsManager {
public:
    // Create an asset manager using only the builtin asset table
    AssetsManager();
    // Create an asset manager using 
    // the builtin asset table and a user defined asset table
    AssetsManager(std::filesystem::path asset_table_path);
    // Create an asset manager using 
    // the builtin asset table and list of user defined asset tables
    AssetsManager(std::vector<std::filesystem::path> asset_table_paths);
    
    // Get an asset handle from the given asset name or file path
    template<typename AssetType>
    AssetHandle<AssetType> get(std::string_view asset_name);

private:
    internal::AssetParser<Shader, Texture, Mesh> m_parser;
    internal::AssetsCache<Shader, Texture, Mesh> m_cache;
};
    
// Get an asset handle from the given asset name or file path
template<typename AssetType>
AssetHandle<AssetType> AssetsManager::get(std::string_view asset_name)
{
    return m_cache.getHandle<AssetType>(
        m_parser,
        asset_name
    );
}

} // namespace cndt

#endif
