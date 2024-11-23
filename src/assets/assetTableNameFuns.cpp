#include "conduit/assets/mesh.h"
#include "conduit/assets/shader.h"

#include "conduit/assets/texture.h"

#include "conduit/assets/assetsTypeFuns.h"

namespace cndt {

template <>
std::string assetTableName<cndt::Shader>() {
    return "shaders";
}
template <>
std::string assetTableName<cndt::Mesh>() {
    return "meshes";
}
template <>
std::string assetTableName<cndt::Texture>() {
    return "textures";
}

}
