#ifndef CNDT_ECS_ENTITY_H
#define CNDT_ECS_ENTITY_H

#include "conduit/defines.h"
#include <cstdint>

namespace cndt {

namespace internal {

class EntityRegister;

} // namespace cndt::internal

// ECS entity type definition 
class Entity {
    friend class internal::EntityRegister;

public:
    using EntityId = u64;
    
public:
    // Public Entity constructor, return an invalid entity
    Entity() : m_id(UINT64_MAX) { }

    // Get the unique entity id
    EntityId id() const { return m_id; }

    // Return true if the entity is invalid
    bool invalid() const { return (m_id == UINT64_MAX); };
    
    friend bool operator==(const Entity& lhs, const Entity& rhs) 
    { 
        return lhs.m_id == rhs.m_id; 
    }
    friend bool operator!=(const Entity& lhs, const Entity& rhs) 
    { 
        return lhs.m_id != rhs.m_id; 
    }
    
    // Ordering for std map
    friend bool operator<(const Entity& lhs, const Entity& rhs) 
    {
        return lhs.m_id < rhs.m_id;
    }

private:
    // Private constructor callable only by the friend entity register  
    explicit Entity(EntityId entity) : m_id(entity) { }
    
private:
    EntityId m_id;
};

} // namespace cndt

#endif
