#ifndef CNDT_ASSET_PARSER_H
#define CNDT_ASSET_PARSER_H

#include "conduit/assets/assetsManagerException.h"
#include "conduit/assets/assetInfo.h"
#include "conduit/assets/assetsTypeFuns.h"
#include "conduit/defines.h"
#include "conduit/logging.h"

#include <filesystem>
#include <fstream>
#include <optional>
#include <tuple>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <fmt/format.h>

namespace cndt::internal {

using json = nlohmann::json;

template<typename... AssetTypes>
class AssetParser {
private:
    static constexpr const char* builtin_table_path =
        "resources/builtin_table.json";
    
private:
    // Store the asset table
    template<typename AssetType>
    class Table {
    public:
        // Create an empty table
        Table() = default;
        // Load the table from a json file
        Table(json type_table);
        
        // Get the asset info in the map
        // Return an empty option if the asset doesn't exist
        std::optional<AssetInfo<AssetType>> getInfo(
            std::string_view asset_name
        );
        
    private:
        using Key = std::string;
        
        // Store the table in memory at all times
        std::unordered_map<Key, AssetInfo<AssetType>> m_table_map;
    };
    
    using Tables = std::tuple<Table<AssetTypes>...>;

public:
    // Create the asset allocator using only the builtin asset table
    AssetParser();
    // Create the asset allocator using 
    // the builtin asset table and a list of user defined asset tables
    AssetParser(std::vector<std::filesystem::path> asset_table_paths);
    
    // Find an asset check if it exist and return a valid asset info struct
    // Take asset_name as input
    // Throw an asset not found exception if the asset doesn't exist 
    template<typename AssetType>
    std::optional<AssetInfo<AssetType>> getInfo(std::string_view asset_name);

private:
    // Create a tables tuple from the given file path
    Tables createTable(std::filesystem::path table_path);

private:
    // Json asset tables
    std::vector<Tables> m_tables;
};

/*
 *
 *      Table template implementation
 *
 * */

// Load the table from a json file
template<typename... AssetTypes>
template<typename AssetType>
AssetParser<AssetTypes...>::Table<AssetType>::Table(json type_table) {
    for (auto& element : type_table.items()) {
        // Catch exception during parsing and print them to screen
        // in case of error no asset will be added to the table
        try {
            Table::Key key(element.key());

            log::core::info("{}: {}", assetTableName<AssetType>(), key);

            // Check for duplicate element 
            // (Should never happen with current json lib)
            if (m_table_map.find(key) != m_table_map.end()) {
                log::core::error(
                    "Duplicate asset \"{}\" in {}; {}",
                    key,
                    assetTableName<AssetType>(),
                    "Using the last declared asset"
                );
            }

            // Add the entry to the table
            m_table_map[key] = 
                parseTableEntry<AssetType>(
                    element.key(),
                    element.value()
                );

        } catch (std::exception &e) {
            log::core::error(
                "Error while parsing {} asset \"{}\": {}", 
                assetTableName<AssetType>(),
                element.key(),
                e.what()
            );
        }
    }
}

// Get the asset info in the map
// Return an empty option if the asset doesn't exist
template<typename... AssetTypes>
template<typename AssetType>
std::optional<AssetInfo<AssetType>>
AssetParser<AssetTypes...>::Table<AssetType>::getInfo(
    std::string_view asset_name
) {
    std::string key(asset_name);

    if(m_table_map.find(key) != m_table_map.end()) {
        return m_table_map[key];
    }
    
    return std::nullopt;
}

/*
 *
 *      Asset locator template implementation
 *
 * */

// Create the asset allocator using only the builtin asset table
template<typename... AssetTypes>
AssetParser<AssetTypes...>::AssetParser() {
    m_tables.push_back(createTable(builtin_table_path));
}

// Create the asset allocator using 
// the builtin asset table and a list of user defined asset tables
template<typename... AssetTypes>
AssetParser<AssetTypes...>::AssetParser(
    std::vector<std::filesystem::path> asset_table_paths
) {
    m_tables.push_back(createTable(builtin_table_path));

    // Add the user tables
    for (auto& path : asset_table_paths) {
        // For user table error warn the user but catch exception
        try {
            m_tables.emplace_back(createTable(path));

        } catch (std::exception &e) {
            log::core::error(
                "Error while parsing asset table \"{}\": {} - ({})",
                path.string(),
                e.what(),
                "All the table assets will be ingored"
            );
        }
    }
}

// Find an asset check if it exist and return a valid asset info struct
// Take asset_name as input
// Throw an asset not found exception if the asset doesn't exist 
template<typename... AssetTypes>
template<typename AssetType>
std::optional<AssetInfo<AssetType>> AssetParser<AssetTypes...>::getInfo(
    std::string_view asset_name
) {
    // Look for the asset in the tables starting from the last
    // (The user define table are more likely to store the asset)
    for (usize i = m_tables.size() - 1; i >= 0; i--) {
        auto info = std::get<Table<AssetType>>(m_tables[i])
            .getInfo(asset_name);

        if (info.has_value()) {
            return info.value();
        }
    }

    // Return nullopt if the asset was not found anywhere
    return std::nullopt;
}

// Create a tables tuple from the given file path
template<typename... AssetTypes>
typename AssetParser<AssetTypes...>::Tables
AssetParser<AssetTypes...>::createTable(
    std::filesystem::path table_path
) {
    if(!std::filesystem::exists(table_path)) {
        throw AssetTableNotFound(
            "Asset table at: \"{}\" not found",
            table_path.string()
        );
    }
    
    try {
        // Read the json file
        std::ifstream f(table_path);
        json table = json::parse(f);

        return std::make_tuple(
            Table<AssetTypes>(table.at(assetTableName<AssetTypes>()))...
        );
        
    } catch (std::exception &e) {
        throw AssetTableParseError(
            "Asset table ({}) parse error: {}",
            table_path.string(),
            e.what()
        );
    }
}

} // namespace cndt::internal

#endif
