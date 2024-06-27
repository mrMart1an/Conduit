#include "conduit/assets/assetsManager.h"

namespace cndt {

// Create an asset manager using only the builtin asset table
AssetsManager::AssetsManager() 
{ }

// Create an asset manager using 
// the builtin asset table and a user defined asset table
AssetsManager::AssetsManager(std::filesystem::path asset_table_path) 
{ }

} // namespace cndt

