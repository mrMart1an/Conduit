#ifndef CNDT_ASSET_MANAGER_EXCEPTION_H
#define CNDT_ASSET_MANAGER_EXCEPTION_H

#include "conduit/defines.h"

#include <format>

namespace cndt {

// General asset exception
class AssetException : public Exception {
public:
    AssetException() : Exception("Unknow asset exception") { }      
    AssetException(std::string_view msg) : Exception(msg) { }      
};

// Asset table not found exception
class AssetTableNotFound : public AssetException {
public:
    AssetTableNotFound(
        std::string_view msg
    ) : AssetException(msg) { }      
};

// Asset table parse exception
class AssetTableParseError : public AssetException {
public:
    AssetTableParseError(
        std::string_view msg
    ) : AssetException(msg) { }      
};

// Asset unavailable exception
class AssetUnavailable : public AssetException {
public:
    AssetUnavailable(
        std::string_view msg,
        std::string_view asset_name
    ) : AssetException(std::format(
        "asset {}: {}",
        asset_name,
        msg
    )) { }      
};

} // namespace cndt

#endif
