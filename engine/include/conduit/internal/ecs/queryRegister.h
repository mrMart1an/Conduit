#ifndef CNDT_ECS_QUERY_REGISTER_H
#define CNDT_ECS_QUERY_REGISTER_H

#include "conduit/ecs/query.h"
#include "conduit/internal/ecs/QueryTypeRegister.h"
#include "conduit/internal/ecs/componentRegister.h"
#include "conduit/internal/ecs/queryStorage.h"

#include <map>
#include <memory>
#include <shared_mutex>
#include <tuple>

namespace cndt::internal {


// Cache query storage object for each type combination and return query
class QueryRegister {
public:
    // Get a query from one of the cashed storage if it exist
    // or create a new query storage if it doesn't
    template<typename... CompTypes>
    Query<CompTypes...> getQuery(ComponentRegister &comp_register);
    
private:
    // Add the query storage to the register if it doesn't already exist
    template<typename... CompTypes>
    void addQueryStorage(ComponentRegister &comp_register);
    
private:
    std::shared_mutex m_mutex;
    
    // Store generic unique pointer to the query storage
    using TypeId = QueryTypeRegister::TypeId;
    using QueryStoragePtr = std::unique_ptr<QueryStorageBase>;
    
    std::map<TypeId, QueryStoragePtr> m_query_storages;
};

// Get a query from one of the cashed storage if it exist
// or create a new query storage if it doesn't
template<typename... CompTypes>
Query<CompTypes...> QueryRegister::getQuery(ComponentRegister &comp_register)
{
    // Add a query storage if it doesn't already exist
    addQueryStorage<CompTypes...>(comp_register);
    
    // Get the unique component type id
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    auto type_id = QueryTypeRegister::getTypeId<CompTypes...>();
    
    auto storage_p = static_cast<QueryStorage<CompTypes...>*>(
        m_query_storages[type_id].get()
    );

    return storage_p->createQuery();
}

// Add the query storage to the register if it doesn't already exist
template<typename... CompTypes>
void QueryRegister::addQueryStorage(ComponentRegister &comp_register) 
{
    // Get the unique component type id
    auto type_id = QueryTypeRegister::getTypeId<CompTypes...>();
    
    // Check if the component buffer already exist
    bool type_exist = false;
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        type_exist = m_query_storages.find(type_id) != m_query_storages.end();
    }

    // If the component buffer doesn't exist lock the mutex and 
    // add the component to the register
    if (!type_exist) {
        std::lock_guard<std::shared_mutex> lock(m_mutex);
        
        auto buffer = std::make_unique<QueryStorage<CompTypes...>>(
            std::make_tuple(
                comp_register.getComponentBuffer<CompTypes>()...
            )
        );
        
        m_query_storages[type_id] = std::move(buffer);
    }
}

} // namespace cndt::internal

#endif
