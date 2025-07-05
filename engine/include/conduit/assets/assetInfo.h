#ifndef CNDT_ASSET_INFO_H
#define CNDT_ASSET_INFO_H

#include <string_view>

namespace cndt {

// Store the asset base information
class AssetInfoBase {
public:
    AssetInfoBase(std::string_view asset_name) : 
        m_asset_name(asset_name) 
    { }
    AssetInfoBase() : 
        m_asset_name("") 
    { }

    // Return the asset name
    std::string_view assetName() const { return m_asset_name; }

private:
    // Store the asset name or path
    std::string m_asset_name;
};

// General asset info template
template<typename AssetType>
class AssetInfo;

} // namespace cndt

#endif
