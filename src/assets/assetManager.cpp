#include "conduit/assets/assetsManager.h"

#include "conduit/internal/assets/assetParserFuns.h"

namespace cndt {

// Create an asset manager using only the builtin asset table
AssetsManager::AssetsManager() :
    m_parser(
        {
            {"shaders" ,internal::parseShader},
            {"textures", internal::parseTexture},
            {"meshes", internal::parseMesh}
        }
    )
{ }

// Create an asset manager using 
// the builtin asset table and a user defined asset table
AssetsManager::AssetsManager(std::filesystem::path asset_table_path) : 
    m_parser(
        asset_table_path,
        {
            {"shaders" ,internal::parseShader},
            {"textures", internal::parseTexture},
            {"meshes", internal::parseMesh}
        }
    )
{ }

} // namespace cndt

