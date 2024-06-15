#ifndef CNDT_ECS_QUERY_H
#define CNDT_ECS_QUERY_H

#include "conduit/ecs/queryElement.h"

#include "conduit/internal/ecs/componentBuffer.h"

#include <vector>

namespace cndt {

// TODO: make query thread safe

// ECS query
template <typename... CompTypes>
class Query {
private:
    template<typename CompType>
    using Buffer = internal::ComponentBuffer<CompType>;
    
    using BufferLock = std::shared_lock<std::shared_mutex>;
    
public:
    static constexpr usize components_count = sizeof...(CompTypes);

    // Query iterator
    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = QueryElement<CompTypes...>;
        using pointer           = value_type*;
        using reference         = value_type&;
        
        // Iterator default constructor
        iterator(
            std::vector<QueryElement<CompTypes...>>::iterator m_elements_iter
        ) : m_elements_iter(m_elements_iter) { } 
        
        // Prefix increment
        iterator& operator++() 
        { 
            m_elements_iter += 1;
            return *this; 
        }  

        // Postfix increment
        iterator operator++(int) 
            { iterator tmp = *this; ++(*this); return tmp; }
        
        reference operator*() const 
        { 
            return m_elements_iter; 
        }
        pointer operator->() 
        { 
            return &m_elements_iter; 
        }
        
        friend bool operator== (const iterator& a, const iterator& b) 
            { return a.m_elements_iter == b.m_elements_iter; };
        friend bool operator!= (const iterator& a, const iterator& b) 
            { return a.m_elements_iter != b.m_elements_iter; };   
        
    private:
        // Store an iterator over the elements list
        std::vector<QueryElement<CompTypes...>>::iterator m_elements_iter;
    }; 
    
public:
    // Query constructors 
    Query<CompTypes>() = default;
    Query<CompTypes>(
        std::vector<QueryElement<CompTypes...>>& elements
    ) : m_elements(elements) { }

    // Get an iterator stating at the beginning of the components list
    iterator begin();
     
    // Get an iterator to the end of the components list
    iterator end();

private:
    // Store a list of query element
    std::vector<QueryElement<CompTypes...>> &m_elements;
};

// Get an iterator stating at the beginning of the components list
template <typename... CompTypes>
Query<CompTypes...>::iterator Query<CompTypes...>::begin()
{
    return iterator(m_elements.begin());
}
 
// Get an iterator to the end of the components list
template <typename... CompTypes>
Query<CompTypes...>::iterator Query<CompTypes...>::end()
{
    return iterator(m_elements.end());
}

} // namespace cndt

#endif
