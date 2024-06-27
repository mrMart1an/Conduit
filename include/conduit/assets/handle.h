#ifndef CNDT_ASSET_HANDLE_H
#define CNDT_ASSET_HANDLE_H

#include "conduit/assets/assetInfo.h"
#include "conduit/internal/assets/assetStorage.h"

#include <algorithm>
#include <memory>

namespace cndt {

// Store a reference to an asset
template<typename AssetType>
class Handle {
public:
    Handle() : 
        m_ptr(nullptr), 
        m_old_version(0)
    { }
    Handle(
        std::shared_ptr<AssetStorage<AssetType>> asset_p
    ) : 
        m_ptr(std::move(asset_p)), 
        m_old_version(0)
    { }

    // Return true if the handle point to an available asset
    bool isAvailable() const { return m_ptr->info().isAvailable(); }

    // Return true if the asset was update since the last time
    // this function was called
    bool wasUpdated()
    {
        u64 new_version = m_ptr->info().version();
        
        if (m_old_version != new_version) {
            m_old_version = new_version;
            
            return true;
        }
            
        return false;
    }

    // Return a const reference asset info struct
    const AssetInfo<AssetType>& info() const { return m_ptr->info(); }
     
    // Overload the dereferencing operator
    const AssetType& operator * () const 
    { 
        return m_ptr->asset(); 
    }
    
    // Overloading arrow operator so that
    const AssetType* operator->() const 
    { 
        return m_ptr->asset(); 
    }
    
private:
    // Store a pointer to the asset
    std::shared_ptr<AssetStorage<AssetType>> m_ptr;

    // Store the last asset version since wasUpdated was called
    u64 m_old_version;
};

} // namespace cndt

#endif
