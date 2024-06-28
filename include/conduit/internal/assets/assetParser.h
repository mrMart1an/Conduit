#ifndef CNDT_ASSET_PARSER_H
#define CNDT_ASSET_PARSER_H

#include "conduit/assets/assetsManagerException.h"
#include "conduit/assets/assetInfo.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <tuple>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <utility>

namespace cndt::internal {

using json = nlohmann::json;

template<typename... AssetTypes>
class AssetParser {
private:
    static constexpr const char* builtin_table_path =
        "resources/builtin_table.json";
   
public:
    template <typename AssetType>
    using ParserFun = 
        std::function<AssetInfo<AssetType>(std::string_view, json)>;

    // Store the parsing function for each type
    using ParserFuns = std::tuple<
        std::pair<std::string_view, ParserFun<AssetTypes>>
    ...>;
    
private:
    // Store the asset table
    template<typename AssetType>
    class Table {
    public:
        // Create an empty table
        Table() = default;
        // Load the table from a json file
        Table(
            json type_table,
            ParserFun<AssetType> parser
        );
        
        // Get the asset info in the map
        // Return an empty option if the asset doesn't exist
        std::optional<AssetInfo<AssetType>> getInfo(
            std::string_view asset_name
        );
        
    private:
        using Key = std::string_view;
        
        // Store the table in memory at all times
        std::unordered_map<Key, AssetInfo<AssetType>> m_table_map;
    };
    
    using Tables = std::tuple<Table<AssetTypes>...>;

public:

    
    // Create the asset allocator using only the builtin asset table
    AssetParser(ParserFuns parser_funs);
    // Create the asset allocator using 
    // the builtin asset table and a user defined asset table
    AssetParser(
        std::filesystem::path asset_table_path,
        
        ParserFuns parser_funs
    );
    
    // Find an asset check if it exist and return a valid asset info struct
    // Take asset_name as input
    // Throw an asset not found exception if the asset doesn't exist 
    template<typename AssetType>
    AssetInfo<AssetType> getInfo(std::string_view asset_name);

private:
    // Create a tables tuple from the given file path
    template <usize... Is>
    Tables createTable(
        std::filesystem::path table_path,
        
        ParserFuns parser_funs,
        
        std::index_sequence<Is...>
    );

private:
    // Json builtin asset table
    Tables m_builtin_table;

    // Json user defined asset table
    Tables m_user_table;
    
    // Store the type names for logging purpose
    std::array<std::string_view, sizeof...(AssetTypes)> m_type_names;
};

/*
 *
 *      Table template implementation
 *
 * */

// Load the table from a json file
template<typename... AssetTypes>
template<typename AssetType>
AssetParser<AssetTypes...>::Table<AssetType>::Table(
    json type_table,
    std::function<AssetInfo<AssetType>(std::string_view, json)> parser_fun
) {
    for (auto& element : type_table.items()) {
        m_table_map[element.key()] = parser_fun(
            element.key(),
            element.value()
        );
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
    if(m_table_map.find(asset_name) != m_table_map.end()) {
        return m_table_map[asset_name];
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
AssetParser<AssetTypes...>::AssetParser(
    ParserFuns parser_funs
) {
    m_type_names = {
        std::get<std::pair<std::string_view, ParserFun<AssetTypes>>>(
            parser_funs
        ).first...
    };
    
    m_builtin_table = createTable(
        builtin_table_path, 
        parser_funs, 
        std::index_sequence_for<AssetTypes...>{}
    );
}

// Create the asset allocator using 
// the builtin asset table and a user defined asset table
template<typename... AssetTypes>
AssetParser<AssetTypes...>::AssetParser(
    std::filesystem::path asset_table_path,
    
    ParserFuns parser_funs
) {
    m_type_names = {
        std::get<std::pair<std::string_view, ParserFun<AssetTypes>>>(
            parser_funs
        ).first...
    };

    m_builtin_table = createTable(
        builtin_table_path, 
        parser_funs, 
        std::index_sequence_for<AssetTypes...>{}
    );

    m_user_table = createTable(
        asset_table_path, 
        parser_funs, 
        std::index_sequence_for<AssetTypes...>{}
    );
}

// Helper template to find the index of a type in a parameter pack
template <typename T, typename... Types>
struct IndexOf;

// Find an asset check if it exist and return a valid asset info struct
// Take asset_name as input
// Throw an asset not found exception if the asset doesn't exist 
template<typename... AssetTypes>
template<typename AssetType>
AssetInfo<AssetType> AssetParser<AssetTypes...>::getInfo(
    std::string_view asset_name
) {
    // Look for the asset in the user asset table
    auto location = std::get<Table<AssetType>>(
        m_user_table
    ).getInfo(asset_name);

    if (location.has_value()) {
        return location.value();
    } 
    
    // If the asset was not found in the user asset table 
    // look for it in the builtin asset table
    location = std::get<Table<AssetType>>(
        m_builtin_table
    ).getInfo(asset_name);

    if (location.has_value()) {
        return location.value();
    } 

    // If the asset was not found in any table throw an exception
    throw AssetNotFound(std::format(
        "{} asset \"{}\" not found",
        m_type_names[IndexOf<AssetType, AssetTypes...>::value],
        asset_name
    ));
}

// Create a tables tuple from the given file path
template<typename... AssetTypes>
template <usize... Is>
AssetParser<AssetTypes...>::Tables AssetParser<AssetTypes...>::createTable(
    std::filesystem::path table_path,
    
    ParserFuns parser_funs,
    
    std::index_sequence<Is...>
) {
    if(!std::filesystem::exists(table_path)) {
        throw AssetTableNotFound(std::format(
            "Asset table at: {} not found",
            table_path.string()
        ));
    }
    
    try {
        // Read the json file
        std::ifstream f(table_path);
        json table = json::parse(f);

        return std::make_tuple(
            Table<AssetTypes>(
                table.at(std::get<Is>(parser_funs).first),
                std::get<Is>(parser_funs).second
            )...
        );
        
    } catch (std::exception &e) {
        throw AssetTableParseError(std::format(
            "Asset table ({}) parse error: {}",
            table_path.string(),
            e.what()
        ));
    }
}

// Recursive case
template <typename T, typename First, typename... Rest>
struct IndexOf<T, First, Rest...> {
    static constexpr usize value = 1 + IndexOf<T, Rest...>::value;
};

// Base case
template <typename T, typename... Rest>
struct IndexOf<T, T, Rest...> {
    static constexpr usize value = 0;
};

} // namespace cndt::internal

#endif
