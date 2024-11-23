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
template <typename... AssetTypes>
class AssetsManager {
public:
    // Create an asset manager using only the builtin asset table
    AssetsManager() : m_parser(), m_cache() { };
    // Create an asset manager using 
    // the builtin asset table and a user defined asset table
    AssetsManager(std::filesystem::path asset_table_path) :
        m_parser({asset_table_path}), m_cache() { };
    // Create an asset manager using 
    // the builtin asset table and list of user defined asset tables
    AssetsManager(std::vector<std::filesystem::path> asset_table_paths) :
        m_parser(asset_table_paths), m_cache() { };
    
    // Get an asset handle from the given asset name or file path
    template<typename AssetType>
    AssetHandle<AssetType> get(std::string_view asset_name);

private:
    internal::AssetParser<Shader, Texture, Mesh> m_parser;
    internal::AssetsCache<Shader, Texture, Mesh> m_cache;
};
    
// Get an asset handle from the given asset name or file path
// If the asset is not found return an handle to an unavailable asset
template <typename... AssetTypes>
template<typename AssetType>
AssetHandle<AssetType> AssetsManager<AssetTypes...>::get(
    std::string_view asset_name
) {
    auto info = m_parser.getInfo<AssetType>(asset_name);

    return m_cache.getHandle<AssetType>(
        asset_name,
        info
    );
}

} // namespace cndt

#endif
