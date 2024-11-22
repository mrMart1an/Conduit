#ifndef CNDT_ASSET_INFO_H
#define CNDT_ASSET_INFO_H

#include "conduit/assets/shader.h"
#include "conduit/defines.h"

#include <filesystem>
#include <string_view>

namespace cndt {

// Store the asset base information
class AssetInfoBase {
public:
    AssetInfoBase(std::string_view asset_name) : 
        m_available(false),
        m_version(0),
        m_asset_name(asset_name) 
    { }
    AssetInfoBase() : 
        m_available(false),
        m_version(0),
        m_asset_name("Unknow asset") 
    { }

    // Return true if the asset is available
    bool isAvailable() const { return m_available; }

    // Return the current loaded asset version
    u64 version() const { return m_version; }

    // Return the asset name
    std::string_view assetName() const { return m_asset_name; }

    // Increment the asset version 
    void incrementVersion() { m_version += 1; }
    
    // Set the availability of the asset
    void setAvailable(bool available) { m_available = available; }

private:
    // The asset is loaded and available
    bool m_available;

    // The asset version, incremented by one for each update
    u64 m_version;

    // Store the asset name or path
    std::string m_asset_name;
};

// General asset info template
template<typename AssetType>
class AssetInfo : public AssetInfoBase 
{ 
    CNDT_STATIC_ERROR(
        "AssetInfo not immplemented for this type"
    );
};

// Store a shader asset information
template<>
class AssetInfo<Shader> : public AssetInfoBase {
public:
    AssetInfo() = default;
    AssetInfo(
        std::string_view asset_name,
        
        std::filesystem::path vk_spv,
        std::filesystem::path vk_glsl,

        std::filesystem::path gl_glsl,

        Shader::Type shader_type
    ) : 
        AssetInfoBase(asset_name),
        m_vk_spv(vk_spv),
        m_vk_glsl(vk_glsl),
        m_gl_glsl(gl_glsl),
        m_shader_type(shader_type)
    { }

    // Return the shader type
    Shader::Type shaderType() const { return m_shader_type;} 

    // Get the vulkan spv code path
    std::filesystem::path pathVkSpv() const { return m_vk_spv; }
    // Get the vulkan glsl code path
    std::filesystem::path pathVkGlsl() const { return m_vk_spv; }
    
    // Get the OpenGL glsl code
    std::filesystem::path pathGlGlsl() const { return m_gl_glsl; }

private:
    std::filesystem::path m_vk_spv;
    std::filesystem::path m_vk_glsl;

    std::filesystem::path m_gl_glsl;

    Shader::Type m_shader_type;
};

class Texture;

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

class Mesh;

// Store a texture asset information
template<>
class AssetInfo<Mesh> : public AssetInfoBase {
public:
    enum class FileType {
        Undefined,
        Obj,
        Gltf
    };

public:
    AssetInfo() = default;
    AssetInfo(
        std::string_view asset_name,
        
        std::filesystem::path mesh_path,
        FileType file_type
    ) : 
        AssetInfoBase(asset_name),
        m_mesh_path(mesh_path),
        m_file_type(file_type)
    { }

    // Get the mesh file path
    std::filesystem::path path() const { return m_mesh_path; }
    
    // Get the mesh file type
    FileType fileType() const { return m_file_type; }
    
private:
    std::filesystem::path m_mesh_path;
    
    FileType m_file_type;
};

} // namespace cndt

#endif
