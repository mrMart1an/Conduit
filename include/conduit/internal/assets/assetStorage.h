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
        AssetInfo<AssetType> asset_info,
        std::unique_ptr<AssetType> asset
    ) : 
        m_asset(std::move(asset)),
        m_info(asset_info)
    { };
    AssetStorage() :
        m_asset(nullptr),
        m_info()
    { };
    
    // Get a constant reference to the asset
    const AssetType* asset() const { 
        if (!m_info.isAvailable()) {
            throw AssetUnavailable(
                "Pointer access attempted on unavailable asset \"{}\"",
                m_info.assetName()
            );
        }
        
        return m_asset.get(); 
    }

    // Get a const reference to the asset info
    const AssetInfo<AssetType>& info() const { return m_info; }

private:
    // Get a reference to the asset info
    AssetInfo<AssetType>& infoRef() { return m_info; }
    
    // Update the stored asset
    void updateAsset(
        AssetInfo<AssetType> asset_info,
        std::unique_ptr<AssetType> asset
    ) {
        m_asset = std::move(asset);
        m_info = asset_info;
    }   
    
private:
    // Store the asset in a unique pointer
    std::unique_ptr<AssetType> m_asset;
    
    // Store the asset information
    AssetInfo<AssetType> m_info;
};

} // namespace cndt

#endif
