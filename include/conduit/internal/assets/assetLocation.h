#ifndef CNDT_ASSET_LOCATION_H
#define CNDT_ASSET_LOCATION_H

namespace cndt::internal {

// Store the location of a conduit asset
struct AssetLocation {
public:
    AssetLocation() = default;
    AssetLocation(std::filesystem::path file_path) :
        m_asset_path(file_path) { }

    // Get the file path of the asset
    std::filesystem::path path() const { return m_asset_path; }
    
private:
    // Store the path of the asset on the disk
    std::filesystem::path m_asset_path;
};

}

#endif
