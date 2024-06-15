#ifndef CNDT_ECS_QUERY_ELEMENT_H
#define CNDT_ECS_QUERY_ELEMENT_H

#include "conduit/ecs/entity.h"

#include <tuple>

namespace cndt {

template <typename... CompTypes>
class QueryElement {
public:
    QueryElement() = default;
    QueryElement(
        Entity entity,
        std::tuple<CompTypes&...> components
    ) : m_entity(entity), m_components(components) { }
    
    // Get the entity associated to the components tuple
    Entity entity() const { return m_entity; }

    // Get the components tuple using the dot operators
    std::tuple<CompTypes&...> operator*() const 
    { 
        return m_components; 
    }
    
    friend bool operator== (const QueryElement& a, const QueryElement& b) 
        { return a.m_entity == b.m_entity; };
    friend bool operator!= (const QueryElement& a, const QueryElement& b) 
        { return a.m_entity != b.m_entity; };   
    
private:
    // The entity the components are associated to
    Entity m_entity;
    
    // Store a tuple of components references
    std::tuple<CompTypes&...> m_components;
};

} // namespace cndt

#endif
