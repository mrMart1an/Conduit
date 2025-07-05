#ifndef CNDT_ASSETS_FUNS_H
#define CNDT_ASSETS_FUNS_H

#include "conduit/assets/assetInfo.h"

#include <nlohmann/json.hpp>

namespace cndt {

// Template declaration for asset table name function
//
// return a string with the asset table name
template <typename AssetType>
std::string assetTableName();

// Template declaration for asset table parser name function
//
// Parse an entry of the asset type for the given type
// and return a compiled asset info struct
template <typename AssetType>
AssetInfo<AssetType> parseTableEntry(std::string_view, nlohmann::json);


// Template declaration for asset loader function
//
// Load the asset from the data in the given asset info and
// return a shared pointer to the asset an asset storage function 
// containing the asset
template <typename AssetType>
std::unique_ptr<AssetType> loadAsset(AssetInfo<AssetType>&); 

} // namespace cndt

#endif
