#ifndef CNDT_ASSET_PARSER_FUNS_H
#define CNDT_ASSET_PARSER_FUNS_H

#include "conduit/assets/assetInfo.h"

#include "nlohmann/json_fwd.hpp"

namespace cndt::internal {

// Parse shader info from json 
AssetInfo<Shader> parseShader(std::string_view, nlohmann::json); 

// Parse texture info from json 
AssetInfo<Texture> parseTexture(std::string_view, nlohmann::json); 

// Parse mesh info from json 
AssetInfo<Mesh> parseMesh(std::string_view, nlohmann::json); 

} // namespace cndt::internal

#endif
