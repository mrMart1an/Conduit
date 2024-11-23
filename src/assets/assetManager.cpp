#include "conduit/assets/assetsManager.h"
#include <vector>

namespace cndt {

// Create an asset manager using only the builtin asset table
AssetsManager::AssetsManager() :
    m_parser(),
    m_cache()
{ }

// Create an asset manager using 
// the builtin asset table and a user defined asset table
AssetsManager::AssetsManager(std::filesystem::path asset_table_path) : 
    m_parser({asset_table_path}),
    m_cache()
{ }

// Create an asset manager using 
// the builtin asset table and list of user defined asset tables
AssetsManager::AssetsManager(
    std::vector<std::filesystem::path> asset_table_paths
) : 
    m_parser(asset_table_paths),
    m_cache()
{ }

} // namespace cndt

