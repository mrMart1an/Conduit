#ifndef CNDT_ASSETS_CACHE_H
#define CNDT_ASSETS_CACHE_H

#include "conduit/assets/assetInfo.h"
#include "conduit/logging.h"

#include "conduit/assets/handle.h"
#include "conduit/assets/assetsTypeFuns.h"

#include "conduit/internal/assets/assetStorage.h"

#include <memory>
#include <tuple>
#include <unordered_map>

namespace cndt::internal {

template<typename... AssetTypes>
class AssetsCache {
private:
    // Cached the asset of the given type 
    template<typename AssetType>
    using Cache =
        std::unordered_map<
            std::string_view,
            std::weak_ptr<AssetStorage<AssetType>>
        >;

public:
    AssetsCache() :  m_caches() { }

    // Get an asset handle
    // Load a new asset if this is the asset is not already cached
    template<typename AssetType>
    AssetHandle<AssetType> getHandle(
        std::string_view asset_name,
        std::optional<AssetInfo<AssetType>> asset_info
    );

private:
    // Store the cached asset storage
    std::tuple<Cache<AssetTypes>...> m_caches; 
};

// Get an asset handle
// Load a new asset if this is the asset is not already cached
template<typename... AssetTypes>
template<typename AssetType>
AssetHandle<AssetType> AssetsCache<AssetTypes...>::getHandle(
    std::string_view asset_name,
    std::optional<AssetInfo<AssetType>> asset_info
) {
    Cache<AssetType>& cache = std::get<Cache<AssetType>>(m_caches);
    std::weak_ptr<AssetStorage<AssetType>> storage_p = cache[asset_name];

    // If the asset is cached return an handle to it
    // load the asset in the cache otherwise
    if (!storage_p.expired()) {
        auto storage = storage_p.lock();
        return AssetHandle<AssetType>(storage);
    } else {
        // Get the asset info from the parser and log a message
        log::core::trace(
            "Asset manager: loading asset \"{}\"",
            asset_name
        );

        // If the asset was in the table and has a valid info struct 
        // load it and cache it, otherwise return an unavailable handle
        if (asset_info.has_value()) {
            try {
                // Use the asset type loader to load the asset in the cache
                std::unique_ptr<AssetType> asset_p = 
                    loadAsset<AssetType>(asset_info.value());

                auto new_storage = std::make_shared<AssetStorage<AssetType>>(
                    asset_info.value(),
                    std::move(asset_p)
                );
    
                // Store the asset storage in the cache and return the handle
                cache[asset_name] = new_storage;
    
                return AssetHandle<AssetType>(new_storage);

            } catch (std::exception& e) {
                // In case of exception return a null handle and 
                // warn the user
                log::core::error(
                    "Error during asset \"{}\" loading: {}",
                    asset_name,
                    e.what()
                );

                auto empty_storage =
                    std::make_shared<AssetStorage<AssetType>>();

                // Store the empty storage in case a future asset update
                // make the asset available
                cache[asset_name] = empty_storage;

                return AssetHandle<AssetType>(empty_storage);
            }
        } else {
            auto empty_storage = std::make_shared<AssetStorage<AssetType>>();

            // Store the empty storage in case a future asset update
            // make the asset available
            cache[asset_name] = empty_storage;

            return AssetHandle<AssetType>(empty_storage);
        }
    }
}

} // namespace cndt::internal

#endif
