#ifndef CNDT_ASSET_HANDLE_H
#define CNDT_ASSET_HANDLE_H

#include "conduit/defines.h"

#include "conduit/assets/assetInfo.h"
#include "conduit/internal/assets/assetStorage.h"

#include <memory>

namespace cndt {

// Store a reference to an asset
template<typename AssetType>
class AssetHandle {
public:
    AssetHandle() : 
        m_ptr(nullptr), 
        m_old_version(0)
    { }
    AssetHandle(
        std::shared_ptr<internal::AssetStorage<AssetType>> asset_p
    ) : 
        m_ptr(std::move(asset_p)), 
        m_old_version(0)
    { }

    // Return true if the handle point to an available asset
    bool isAvailable() const { return m_ptr->isAvailable(); }

    // Return true if the asset was update since the last time
    // this function was called
    //
    // return true if the function was never called 
    // and the asset is available
    bool wasUpdated()
    {
        u64 new_version = m_ptr->version();
        
        if (m_old_version != new_version) {
            m_old_version = new_version;
            
            return true;
        }
            
        return false;
    }

    // Return a const reference asset info struct
    const AssetInfo<AssetType>& info() const { return m_ptr->info(); }
     
    // Overload the dereferencing operator
    // Generate exception if the handle is unavailable
    const AssetType& operator * () const 
    { 
        return m_ptr->asset(); 
    }
    
    // Overloading arrow operator so that
    // Generate exception if the handle is unavailable
    const AssetType* operator->() const 
    { 
        return m_ptr->asset(); 
    }
    
private:
    // Store a pointer to the asset
    std::shared_ptr<internal::AssetStorage<AssetType>> m_ptr;

    // Store the last asset version since wasUpdated was called
    u64 m_old_version;
};

} // namespace cndt

#endif
