#ifndef CNDT_ASSETS_CACHE_H
#define CNDT_ASSETS_CACHE_H

#include "conduit/assets/handle.h"
#include "conduit/internal/assets/assetParser.h"
#include "conduit/internal/assets/assetStorage.h"
#include "conduit/logging.h"

#include <functional>
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

    // Function used to load an asset in the cache
    template<typename AssetType>
    using Loader = std::function<
        std::shared_ptr<AssetStorage<AssetType>>(AssetInfo<AssetType>&)
    >;

public:
    AssetsCache() : m_loaders(), m_caches() { }
    AssetsCache(std::tuple<Loader<AssetTypes>...> loaders) : 
        m_loaders(loaders), m_caches()
    { }

    // Get an asset handle
    // Load a new asset if this is the asset is not already cached
    template<typename AssetType>
    AssetHandle<AssetType> getHandle(
        AssetParser<AssetTypes...>& parser,
        std::string_view asset_name
    );

private:
    // Store the asset loading function
    std::tuple<Loader<AssetTypes>...> m_loaders;

    // Store the chaced asset storage
    std::tuple<Cache<AssetTypes>...> m_caches; 
};

// Get an asset handle
// Load a new asset if this is the asset is not already cached
template<typename... AssetTypes>
template<typename AssetType>
AssetHandle<AssetType> AssetsCache<AssetTypes...>::getHandle(
    AssetParser<AssetTypes...>& parser,
    std::string_view asset_name
) {
    Cache<AssetType>& cache = std::get<Cache<AssetType>>(m_caches);
    std::weak_ptr<AssetStorage<AssetType>> storage_p = cache[asset_name];

    // If the asset is cahced return an handle to it
    // load the asset in the cache otherwise
    if (auto storage = storage_p.lock()) {
        return AssetHandle<AssetType>(storage);
    } else {
        // Get the asset info from the parser and log a message
        AssetInfo<AssetType> asset_info = 
            parser.template getInfo<AssetType>(asset_name);

        log::core::trace(
            "Asset manager: loading asset \"{}\"",
            asset_name
        );

        // Use the asset type loader to load the asset in the cache
        Loader<AssetType> loader = std::get<Loader<AssetType>>(m_loaders);
        std::shared_ptr<AssetStorage<AssetType>> new_storage =
            loader(asset_info);

        // Store the asset storage in the cache and return the handle
        cache[asset_name] = new_storage;
        return AssetHandle<AssetType>(new_storage);
    }
}

} // namespace cndt::internal

#endif
