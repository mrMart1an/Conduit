#ifndef CNDT_ECS_QUERY_H
#define CNDT_ECS_QUERY_H

#include "conduit/ecs/queryElement.h"

#include "conduit/internal/ecs/componentBuffer.h"

#include <array>
#include <vector>

namespace cndt {

// TODO: make query actually thread safe

// ECS query
template <typename... CompTypes>
class Query {
private:
    template<typename CompType>
    using Buffer = internal::ComponentBuffer<CompType>;
    
    using BufferLock = std::shared_lock<std::shared_mutex>;
    
    static constexpr usize components_count = sizeof...(CompTypes);

public:
    // Use the underlying vector random access iterator as the query iterator
    using const_iterator = 
        std::vector<QueryElement<CompTypes...>>::const_iterator;
    
public:
    // Query constructors 
    Query<CompTypes>() = default;
    Query<CompTypes>(
        std::vector<QueryElement<CompTypes...>>& elements,
        std::array<BufferLock, components_count>& locks
    ) : m_elements(elements), m_locks(std::move(locks)) { }

    // Get an iterator stating at the beginning of the components list
    const_iterator begin();
     
    // Get an iterator to the end of the components list
    const_iterator end();

    // Return the number of element stored in the query
    usize size() const { return m_elements.size(); }

    // Array operator overload
    QueryElement<CompTypes...> operator [] (usize i) const 
    {
        return m_elements[i]; 
    }

private:
    // Store a list of query element
    std::vector<QueryElement<CompTypes...>> &m_elements;

    // Components buffers shared lock
    std::array<BufferLock, components_count> m_locks;
};

// Get an iterator stating at the beginning of the components list
template <typename... CompTypes>
Query<CompTypes...>::const_iterator Query<CompTypes...>::begin()
{
    return m_elements.cbegin();
}
 
// Get an iterator to the end of the components list
template <typename... CompTypes>
Query<CompTypes...>::const_iterator Query<CompTypes...>::end()
{
    return m_elements.cend();
}

} // namespace cndt

#endif
