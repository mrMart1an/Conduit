#ifndef CNDT_ASSET_HANDLE_H
#define CNDT_ASSET_HANDLE_H

#include <algorithm>
#include <memory>

namespace cndt {

// Store a reference to an asset
template<typename AssetType>
class Handle {
public:
    Handle() = default;
    Handle(std::shared_ptr<AssetType> asset_p) : m_ptr(std::move(asset_p)) { }
    
    // Overload the dereferencing operator
    AssetType& operator * () { return *m_ptr; }
    const AssetType& operator * () const { return *m_ptr; }
    
    // Overloading arrow operator so that
    AssetType* operator->() { return m_ptr; }
    
private:
    // Store a pointer to the asset
    std::shared_ptr<AssetType> m_ptr;
};

} // namespace cndt

#endif
