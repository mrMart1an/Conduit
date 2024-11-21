#ifndef CNDT_ECS_QUERY_STORAGE_H
#define CNDT_ECS_QUERY_STORAGE_H

#include "conduit/ecs/entity.h"
#include "conduit/ecs/query.h"
#include "conduit/ecs/queryElement.h"

#include "conduit/internal/ecs/componentBuffer.h"

#include <array>
#include <memory>
#include <shared_mutex>
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
    using ComponentIter = typename std::vector<CompType>::iterator;
    using EntityIter = std::vector<Entity>::iterator;

public:
    static constexpr usize components_count = sizeof...(CompTypes);

public:
    // Query constructors 
    QueryStorage() = default; 
    QueryStorage(
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

        std::array<usize, components_count> indices,
        std::tuple<ComponentIter<CompTypes>...> components_iter,
        
        std::index_sequence<Is...>
    );

    // Return of a tuple of components iterator 
    template <usize... Is>
    auto componentsBegin(std::index_sequence<Is...>);

    // Return an array of entity iterator at the beginning of the vector
    template <usize... Is>
    auto entityBegin(std::index_sequence<Is...>);
    // Return an array of entity iterator at the end of the vector
    template <usize... Is>
    auto entityEnd(std::index_sequence<Is...>);

    // Return an array of components buffers locks
    template <usize... Is>
    auto componentsLocks(std::index_sequence<Is...>);

private:
    std::tuple<std::weak_ptr<Buffer<CompTypes>>...> m_component_buffers;

    // Store a list of query element
    std::vector<QueryElement<CompTypes...>> m_elements;
    
    // Buffers last version
    u64 m_last_version;
};

// TODO This code is a mess to be fixed as soon as I 
// remember how it actually work

// Update the query element list
template <typename... CompTypes>
void QueryStorage<CompTypes...>::updateQuery()
{
    m_elements.clear();

    // Entity iterator
    constexpr usize iter_count = components_count;
    constexpr std::index_sequence_for<CompTypes...> indices = {};
    
    std::array<EntityIter, iter_count> entity_iter = entityBegin(indices);
    std::array<EntityIter, iter_count> entity_iter_end = entityEnd(indices);

    std::tuple<ComponentIter<CompTypes>...> components_iter = 
        componentsBegin(indices);
    
    // Store a list of indices for the components iterator
    std::array<usize, iter_count> index_list = {};
    
    // Use the element 0 as the algorithm pivot point
    EntityIter& pivot = entity_iter[0];
    EntityIter& pivot_end = entity_iter_end[0];
    usize& pivot_index = index_list[0];

    bool must_run = true;
    while (must_run) {
        Entity pivot_entity = *pivot;

        bool element_found = true;
        usize comp_i = 1;
        while(comp_i < iter_count) {
            EntityIter& iter = entity_iter[comp_i];
            EntityIter& iter_end = entity_iter_end[comp_i];
            usize& index = index_list[comp_i];
            
            Entity entity = *iter;

            // If the current component entity is smaller that the pivot
            // component entity increment the components iterator
            if (entity < pivot_entity) {
                iter += 1;
                index += 1;

                if (iter == iter_end) {
                    element_found = false;
                    break;
                } 

                continue;
            }
            // If the pivot component entity is smaller 
            // that the current component entity increment the pivot iterator
            else if (entity > pivot_entity) {
                pivot += 1;
                pivot_index += 1;
                
                if (pivot == pivot_end) {
                    element_found = false;
                    break;
                } 
                
                // Update the pivot entity and restart the loop 
                // from the first non pivot component
                pivot_entity = *pivot;
                comp_i = 1;
                
                continue;
            }
            
            comp_i += 1;
        }

        // If a component was found add it to the elements list
        if (element_found) {
            m_elements.push_back(createElement(
                pivot_entity,
                index_list,
                components_iter,
                indices
            ));
        } else {
            break;
        }

        // Increment all the iterator to find the next component
        for (usize i = 0; i < iter_count; i++) {
            entity_iter[i] += 1;
            index_list[i] += 1;

            if (entity_iter[i] == entity_iter_end[i])
                must_run = false;
        }
    }
}

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

// Create a query element 
template <typename... CompTypes>
template <usize... Is>
QueryElement<CompTypes...> QueryStorage<CompTypes...>::createElement(
    Entity entity,

    std::array<usize, components_count> indices,
    std::tuple<ComponentIter<CompTypes>...> comp_iter,
    
    std::index_sequence<Is...>
) {
    std::tuple<CompTypes&...> components = std::tie(
        (*(std::get<Is>(comp_iter) + indices[Is]))...
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
    
    std::array<std::shared_lock<std::shared_mutex>, components_count> locks = 
        componentsLocks(std::index_sequence_for<CompTypes...>{});
    
    return Query<CompTypes...>(
        m_elements,
        locks
    );
}

// Return an array of components buffers locks
template <typename... CompTypes>
template <usize... Is>
auto QueryStorage<CompTypes...>::componentsLocks(std::index_sequence<Is...>)
{
    std::array<std::shared_lock<std::shared_mutex>, components_count> locks = 
    {
        (std::get<Is>(m_component_buffers).lock()->lock())...
    }; 
    
    return locks; 
}

} // namespace cndt::internal

#endif
