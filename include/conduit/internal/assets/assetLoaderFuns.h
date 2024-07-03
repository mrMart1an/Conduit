#ifndef CNDT_ASSET_LOADER_FUNS_H
#define CNDT_ASSET_LOADER_FUNS_H

#include "conduit/internal/assets/assetStorage.h"
#include "conduit/assets/assetInfo.h"

namespace cndt::internal {

// Parse shader from the given shader info 
std::shared_ptr<AssetStorage<Shader>> loadShader(AssetInfo<Shader>&); 

// Parse texture from the given texture info 
std::shared_ptr<AssetStorage<Texture>> loadTexture(AssetInfo<Texture>&); 

// Load a mesh from the given mesh info 
std::shared_ptr<AssetStorage<Mesh>> loadMesh(AssetInfo<Mesh>&); 

} // namespace cndt::internal

#endif
