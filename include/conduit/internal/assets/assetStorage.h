#ifndef CNDT_ASSET_STORAGE_H
#define CNDT_ASSET_STORAGE_H

#include "conduit/defines.h"

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
        m_info(asset_info),
        m_available(true),
        m_version(1)
    { };
    AssetStorage() :
        m_asset(nullptr),
        m_info(),
        m_available(false),
        m_version(0)
    { };
    
    // Get a constant reference to the asset
    const AssetType* asset() const { 
        if (!m_available) {
            throw AssetUnavailable(
                "Pointer access attempted on unavailable asset \"{}\"",
                m_info.assetName()
            );
        }

        // Check if the asset pointer is null, should never happen
        if (m_asset == nullptr) {
            throw AssetUnavailable(
                "Asset  \"{}\" pointer in null, this is an engine bug",
                m_info.assetName()
            );
        }

        return m_asset.get(); 
    }

    // Get a const reference to the asset info
    const AssetInfo<AssetType>& info() const { return m_info; }

    // Return true if the asset is available
    bool isAvailable() const { return m_available; }

    // Return the current loaded asset version
    u64 version() const { return m_version; }

private:
    // Get a reference to the asset info
    AssetInfo<AssetType>& infoRef() { return m_info; }
    
    // Update the stored asset
    void updateAsset(
        AssetInfo<AssetType> asset_info,
        std::unique_ptr<AssetType> asset
    ) {
        if (asset != nullptr) {
            m_info = asset_info;
            m_asset = std::move(asset);

            incrementVersion();
            makeAvailable();
        } else {
            makeUnavailable();
        }
    }   

    // Increment the asset version 
    void incrementVersion() { m_version += 1; }
    
    // Set the availability of the asset
    void makeAvailable() { m_available = true; }
    // Set the availability of the asset
    void makeUnavailable() { m_available = false; }
    
private:
    // Store the asset in a unique pointer
    std::unique_ptr<AssetType> m_asset;
    
    // Store the asset information
    AssetInfo<AssetType> m_info;

    // The asset is loaded and available
    bool m_available;
    // The asset version, incremented by one for each update
    u64 m_version;
};

} // namespace cndt

#endif
