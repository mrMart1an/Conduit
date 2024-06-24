#ifndef CNDT_ASSET_STORAGE_H
#define CNDT_ASSET_STORAGE_H

#include "conduit/assets/assetInfo.h"
#include "conduit/assets/assetsManagerException.h"

#include <memory>
#include <utility>

namespace cndt {

// Store the asset and the associated information
template<typename AssetType>
class AssetStorage {
    
public:
    AssetStorage(
        std::string_view asset_name,
        std::unique_ptr<AssetType> asset
    ) : 
        m_asset(std::move(asset)),
        m_info(asset_name)
    { };
    AssetStorage() : 
        m_asset(nullptr),
        m_info()
    { };
    
    // Get a constant reference to the asset
    const AssetType* asset() const { 
        if (!m_info.isAvailable()) {
            throw AssetUnavailable(
                "Pointer access attempted",
                m_info.assetName()
            );
        }
            
        return m_asset.get(); 
    }

    // Get a reference to the asset info
    AssetInfo& info() { return m_info; }
    
private:
    // Store the asset in a unique pointer
    std::unique_ptr<AssetType> m_asset;
    
    // Store the asset information
    AssetInfo m_info;
};

} // namespace cndt

#endif
