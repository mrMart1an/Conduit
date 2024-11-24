#ifndef CNDT_ASSETS_CACHE_H
#define CNDT_ASSETS_CACHE_H

#include "conduit/assets/assetInfo.h"
#include "conduit/internal/assets/assetParser.h"
#include "conduit/logging.h"

#include "conduit/assets/handle.h"
#include "conduit/assets/assetsTypeFuns.h"

#include "conduit/internal/assets/assetStorage.h"

#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>

namespace cndt::internal {

template<typename... AssetTypes>
class AssetsCache {
private:
    // Cached the asset of the given type 
    template<typename AssetType>
    using Cache =
        std::unordered_map<
            std::string,
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

    // Take a parser and update the current cached assets
    void updateAssets(const AssetParser<AssetTypes...>& parser);

private:
    // Update the asset cache of a given type
    template <typename AssetType>
    void updateAssetsCache(const AssetParser<AssetTypes...>& parser);

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
    std::weak_ptr<AssetStorage<AssetType>> storage_p = 
        cache[std::string(asset_name)];

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
                cache[std::string(asset_name)] = new_storage;
    
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
                cache[std::string(asset_name)] = empty_storage;

                return AssetHandle<AssetType>(empty_storage);
            }
        } else {
            auto empty_storage = std::make_shared<AssetStorage<AssetType>>();

            // Store the empty storage in case a future asset update
            // make the asset available
            cache[std::string(asset_name)] = empty_storage;

            return AssetHandle<AssetType>(empty_storage);
        }
    }
}

// Take a parser and update the current cached assets
template<typename... AssetTypes>
void AssetsCache<AssetTypes...>::updateAssets(
    const AssetParser<AssetTypes...>& parser
) {
    // Call update cache for each cache type in the tuple
    auto _ = { 
        (updateAssetsCache<AssetTypes>(parser), 0)... 
    };
}

// Update the asset cache of a given type
template<typename... AssetTypes>
template <typename AssetType>
void AssetsCache<AssetTypes...>::updateAssetsCache(
    const AssetParser<AssetTypes...>& parser
) {
    for (auto& asset_pair : std::get<Cache<AssetType>>(m_caches)) {
        std::string key = asset_pair.first;

        // If the asset is expired don't update it
        if (asset_pair.second.expired()) {
            continue;
        }

        // Lock the shared pointer
        std::shared_ptr<AssetStorage<AssetType>> asset =
            asset_pair.second.lock();

        log::core::debug("Updating asset: \"{}\"", key);

        // Obtain the new asset info from the parser
        std::optional<AssetInfo<AssetType>> info = 
            parser.template getInfo<AssetType>(key);

        // If the asset was found load the new asset 
        if (info.has_value()) {
            try {
                // Try loading the new asset
                std::unique_ptr<AssetType> new_asset = 
                    loadAsset<AssetType>(info.value());
                
                // Update the asset and make it available
                asset->updateAsset(info.value(), std::move(new_asset));

            } catch (std::exception& e) {
                log::core::warn(
                    "The asset \"{}\" is now unavailable (loading error)",
                    key
                );

                // Update the asset and make it unavailable
                asset->updateAsset(AssetInfo<AssetType>(), nullptr);
            }
        } else {
            log::core::warn(
                "The asset \"{}\" is now unavailable (not in the asset table)",
                key
            );

            // Update the asset and make it unavailable
            asset->updateAsset(AssetInfo<AssetType>(), nullptr);
        }
    }
}

} // namespace cndt::internal

#endif
