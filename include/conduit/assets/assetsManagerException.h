#ifndef CNDT_ASSET_MANAGER_EXCEPTION_H
#define CNDT_ASSET_MANAGER_EXCEPTION_H

#include "conduit/exception.h"

#include <utility>

#include "fmt/base.h"

namespace cndt {

// General asset exception
class AssetException : public Exception {
public:
    AssetException() : Exception("Unknow asset exception") { }      

    template<typename... Args>
    AssetException(fmt::format_string<Args...> msg, Args&&... args) 
    : Exception(msg, std::forward<Args>(args)...) { }      
};

// Asset table not found exception
class AssetTableNotFound : public AssetException {
public:
    template<typename... Args>
    AssetTableNotFound(fmt::format_string<Args...> msg, Args&&... args) 
    : AssetException(msg, std::forward<Args>(args)...) { }      
};

// Asset table parse exception
class AssetTableParseError : public AssetException {
public:
    template<typename... Args>
    AssetTableParseError(
        fmt::format_string<Args...> msg, Args&&... args
    ) : AssetException(msg, std::forward<Args>(args)...) { }      
};

// Asset unavailable exception
class AssetUnavailable : public AssetException {
public:
    template<typename... Args>
    AssetUnavailable(
        fmt::format_string<Args...> msg, Args&&... args
    ) : AssetException(msg, std::forward<Args>(args)...) { }      
};

// Asset not found exception
class AssetNotFound : public AssetException {
public:
    template<typename... Args>
    AssetNotFound(
        fmt::format_string<Args...> msg, Args&&... args
    ) : AssetException(msg, std::forward<Args>(args)...) { }      
};

} // namespace cndt

#endif
