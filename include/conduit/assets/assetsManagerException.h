#ifndef CNDT_ASSET_MANAGER_EXCEPTION_H
#define CNDT_ASSET_MANAGER_EXCEPTION_H

#include "conduit/exception.h"

#include <fmt/core.h>

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
    ) : AssetException(fmt::format(
        "asset {}: {}",
        asset_name,
        msg
    )) { }      
};

// Asset not found exception
class AssetNotFound : public AssetException {
public:
    AssetNotFound(
        std::string_view msg
    ) : AssetException(msg) { }      
};

} // namespace cndt

#endif
