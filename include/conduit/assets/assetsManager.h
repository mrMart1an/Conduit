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
    AssetsManager() = default;
    
    // Get an asset handle from the given asset name or file path
    template<typename AssetType>
    Handle<AssetType> get(std::string_view asset_name);

private:
    internal::AssetsCache<Shader> m_shader_cache;
};

} // namespace cndt

#endif