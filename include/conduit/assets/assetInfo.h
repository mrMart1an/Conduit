#ifndef CNDT_ASSET_INFO_H
#define CNDT_ASSET_INFO_H

#include "conduit/defines.h"
#include <string_view>

namespace cndt {

// Store the asset information
class AssetInfo {
public:
    AssetInfo(std::string_view asset_name) : 
        m_available(true),
        m_version(1),
        m_asset_name(asset_name) 
    { }
    AssetInfo() : 
        m_available(false),
        m_version(0),
        m_asset_name("Unknow asset") 
    { }

    // Return true if the asset is available
    bool isAvailable() const { return m_available; }

    // Return the current loaded asset version
    u64 version() const { return m_version; }

    // Return the asset name
    std::string_view assetName() const { return m_asset_name; }

private:
    // Increment the asset version 
    void incrementVersion() { m_version += 1; }
    
    // Set the availability of the asset
    void setAvailable(bool available) { m_available = available; }

private:
    // The asset is loaded and available
    bool m_available;

    // The asset version, incremented by one for each update
    u64 m_version;

    // Store the asset name or path
    std::string m_asset_name;
};

} // namespace cndt

#endif
