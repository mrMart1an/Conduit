#ifndef CNDT_ASSETS_MANAGER_H
#define CNDT_ASSETS_MANAGER_H

#include "conduit/assets/handle.h"

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
    AssetsManager() : m_parser(), m_cache(), m_tables_paths() { };
    // Create an asset manager using 
    // the builtin asset table and a user defined asset table
    AssetsManager(std::filesystem::path asset_table_path) :
        m_parser({asset_table_path}), m_cache(),
        m_tables_paths({asset_table_path}) { };
    // Create an asset manager using 
    // the builtin asset table and list of user defined asset tables
    AssetsManager(std::vector<std::filesystem::path> asset_table_paths) :
        m_parser(asset_table_paths), m_cache(), 
        m_tables_paths(asset_table_paths) { };
    
    // Get an asset handle from the given asset name
    template<typename AssetType>
    AssetHandle<AssetType> get(std::string_view asset_name);

    // Update all the cached asset in the asset manager.
    // This function reload all the asset from memory
    // and can be very slow should be called only when needed
    void updateAssets();

private:
    internal::AssetParser<AssetTypes...> m_parser;
    internal::AssetsCache<AssetTypes...> m_cache;

    // Store the user asset tables paths
    std::vector<std::filesystem::path> m_tables_paths;
};
    
// Get an asset handle from the given asset name or file path
// If the asset is not found return an handle to an unavailable asset
template <typename... AssetTypes>
template<typename AssetType>
AssetHandle<AssetType> AssetsManager<AssetTypes...>::get(
    std::string_view asset_name
) {
    auto info = m_parser.template getInfo<AssetType>(asset_name);

    return m_cache.template getHandle<AssetType>(
        asset_name,
        info
    );
}

// Update all the cached asset in the asset manager.
template <typename... AssetTypes>
void AssetsManager<AssetTypes...>::updateAssets()
{
    // Parse the tables again and store the new parser
    m_parser = internal::AssetParser<AssetTypes...>(m_tables_paths);

    // Update the assets cache
    m_cache.updateAssets(m_parser);
}

} // namespace cndt

#endif
