#ifndef CNDT_ECS_QUERY_STORAGE_H
#define CNDT_ECS_QUERY_STORAGE_H

#include "conduit/ecs/entity.h"
#include "conduit/ecs/query.h"
#include "conduit/ecs/queryElement.h"

#include "conduit/internal/ecs/componentBuffer.h"

#include <array>
#include <functional>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

namespace cndt::internal {

class QueryStorageBase {
public:
    QueryStorageBase() = default;
    virtual ~QueryStorageBase() = default;
};

// ECS query storage
template <typename... CompTypes>
class QueryStorage : public QueryStorageBase {
private:
    template<typename CompType>
    using Buffer = internal::ComponentBuffer<CompType>;
    
    using BufferLock = std::shared_lock<std::shared_mutex>;
    
    template<typename CompType>
    using ComponentIter = std::vector<CompType>::iterator;
    using EntityIter = std::vector<Entity>::iterator;

public:
    static constexpr usize components_count = sizeof...(CompTypes);

public:
    // Query constructors 
    QueryStorage<CompTypes>() = default; 
    QueryStorage<CompTypes>(
        std::tuple<std::weak_ptr<Buffer<CompTypes>>...> buffer_p
    ) : m_component_buffers(buffer_p), m_elements(), m_last_version() { }

public:
    // Return a Query handle from the storage
    Query<CompTypes...> createQuery();

private:
    // Get the buffers version
    u64 buffersVersion();

    // Update the query element list
    void updateQuery();

    // Create a query element 
    // the given iterator need to be at the beginning 
    // of the components buffers
    template <usize... Is>
    QueryElement<CompTypes...> createElement(
        Entity entity,

        usize index,
        std::tuple<ComponentIter<CompTypes>...> components_iter,
        
        std::index_sequence<Is...>
    );

    // Return of a tuple of components iterator 
    template <usize... Is>
    auto componentsBegin(
        std::index_sequence<Is...>
    );

    // Return an array of entity iterator at the beginning of the vector
    template <usize... Is>
    auto entityBegin(
        std::index_sequence<Is...>
    );
    // Return an array of entity iterator at the end of the vector
    template <usize... Is>
    auto entityEnd(
        std::index_sequence<Is...>
    );

private:
    std::tuple<std::weak_ptr<Buffer<CompTypes>>...> m_component_buffers;

    // Store a list of query element
    std::vector<QueryElement<CompTypes...>> m_elements;
    
    // Buffers last version
    u64 m_last_version;
};

// Return of a tuple of components iterator 
template <typename... CompTypes>
template <usize... Is>
auto QueryStorage<CompTypes...>::componentsBegin(
    std::index_sequence<Is...>
) {
    return std::make_tuple(
        (
            std::get<Is>(m_component_buffers).lock()->componentVector().begin()
        )...
    );
}

// Return an array of entity iterator at the beginning of the vector
template <typename... CompTypes>
template <usize... Is>
auto QueryStorage<CompTypes...>::entityBegin(
    std::index_sequence<Is...>
) {
    std::array<EntityIter, components_count> entity_iter = {
        (std::get<Is>(m_component_buffers).lock()->entityVector().begin())...
    }; 
    
    return entity_iter; 
}

// Return an array of entity iterator at the end of the vector
template <typename... CompTypes>
template <usize... Is>
auto QueryStorage<CompTypes...>::entityEnd(
    std::index_sequence<Is...>
) {
    std::array<EntityIter, components_count> entity_iter = {
        (std::get<Is>(m_component_buffers).lock()->entityVector().end())...
    }; 
    
    return entity_iter; 
}

// Update the query element list
template <typename... CompTypes>
void QueryStorage<CompTypes...>::updateQuery()
{
    // Entity iterator
    constexpr usize iter_count = components_count;
    constexpr std::index_sequence_for<CompTypes...> indices = {};
    
    std::array<EntityIter, iter_count> entity_iter = entityBegin(indices);
    std::array<EntityIter, iter_count> entity_iter_end = entityEnd(indices);

    std::tuple<ComponentIter<CompTypes>...> components_iter = 
        componentsBegin(indices);
    
    // Store an entity and the corresponding index in the buffer
    std::array<std::pair<Entity, usize>, iter_count> index_list;
    
    EntityIter& pivot = entity_iter[0];

    while (false) {
        for (int i = 1; i < iter_count; i++) {
            
        }
    }
}

// Create a query element 
template <typename... CompTypes>
template <usize... Is>
QueryElement<CompTypes...> QueryStorage<CompTypes...>::createElement(
    Entity entity,

    usize index,
    std::tuple<ComponentIter<CompTypes>...> comp_iter,
    
    std::index_sequence<Is...>
) {
    std::tuple<CompTypes&...> components = std::make_tuple(
        (std::ref(*(std::get<Is>(comp_iter) + index)))...
    );
    
    return QueryElement<CompTypes...>(entity, components);
}

// Get the buffers version
template <typename... CompTypes>
u64 QueryStorage<CompTypes...>::buffersVersion()
{
    u64 new_version;

    std::apply([&](auto&&... buffers){
        new_version = (buffers.lock()->version() + ...);
    }, m_component_buffers);

    return new_version;
}

// Return a Query handle from the storage
template <typename... CompTypes>
Query<CompTypes...> QueryStorage<CompTypes...>::createQuery()
{
    u64 new_version = buffersVersion();

    if (new_version != m_last_version) {
        updateQuery();
        m_last_version = new_version;
    }
    
    return Query<CompTypes...>(m_elements);
}

} // namespace cndt::internal

#endif
