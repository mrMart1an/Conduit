#ifndef CNDT_TEXTURE_ASSET_H
#define CNDT_TEXTURE_ASSET_H

#include "conduit/assets/assetInfo.h"

namespace cndt {

// Store a conduit texture asset
class Texture {
public:
    Texture() = default;
};

// Store a texture asset information
template<>
class AssetInfo<Texture> : public AssetInfoBase {
public:
    AssetInfo() = default;
    AssetInfo(
        std::string_view asset_name,
        
        std::filesystem::path src_path,
        std::filesystem::path norm_path
    ) : 
        AssetInfoBase(asset_name),
        m_src_path(src_path),
        m_norm_path(norm_path)
    { }

    // Get the source image file path
    std::filesystem::path srcPath() const { return m_src_path; }
    
    // Get the image normal file path
    // this path is guarantied to be valid only if not empty
    std::filesystem::path normPath() const { return m_norm_path; }
    
private:
    std::filesystem::path m_src_path;
    std::filesystem::path m_norm_path;
};

} // namespace cndt

#endif 
