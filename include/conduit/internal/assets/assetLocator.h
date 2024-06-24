#ifndef CNDT_ASSET_LOCATOR_H
#define CNDT_ASSET_LOCATOR_H

#include "conduit/internal/assets/assetLocation.h"

#include <filesystem>
#include <functional>
#include <optional>
#include <unordered_map>
#include <utility>

namespace cndt::internal {

class AssetLocator {
public:
    // Store the asset type
    enum class Type {
        Texture,
        Mesh,
        Shader
    };

private:
    // Store the asset table
    class Table {
    public:
        // Create an empty table
        Table() = default;
        // Load the table from a json file
        Table(std::filesystem::path table_path);

        
        // Get the asset location in the map
        // Return an empty option if the asset doesn't exist
        std::optional<AssetLocation> getLocation(
            Type asset_type,
            std::string_view asset_name
        );
        
    private:
        using Key = std::pair<Type, std::string_view>;
        
        struct keyHash {
            std::size_t operator()(const Key& p) const {
                auto hash1 = std::hash<Type>{}(p.first);
                auto hash2 = std::hash<std::string_view>{}(p.second);
                
                return hash1 ^ (hash2 << 1); // Combine the tow hash
            }
        };
        
        struct keyEqual {
            bool operator()(const Key& p1, const Key& p2) const {
                return (p1.first == p2.first) && (p1.second == p2.second);
            }
        };
        
    private:
        // Store the table in memory at all times
        std::unordered_map<Key, AssetLocation, keyHash, keyEqual> m_table_map;
    };

public:
    // Create the asset allocator using only the builtin asset table
    AssetLocator();
    // Create the asset allocator using 
    // the builtin asset table and a user defined asset locator
    AssetLocator(std::filesystem::path asset_table_pat);
    
    // Find an asset check if it exist and return a valid asset location struct
    // Take asset_name or file path as input
    // If the asset is not found use the first builtin of the same type
    AssetLocation locate(
        Type asset_type,
        std::string_view asset_name
    );

private:
    // Json builtin asset table
    Table m_builtin_table;

    // Json user defined asset table
    Table m_user_table;
};

} // namespace cndt::internal

#endif
