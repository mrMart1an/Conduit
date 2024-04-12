#ifndef CNDT_ECS_ENTITY_H
#define CNDT_ECS_ENTITY_H

#include "conduit/defines.h"

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
    // Get the unique entity id
    EntityId id() const { return m_id; }
    
    friend bool operator==(const Entity& lhs, const Entity& rhs) 
    { 
        return lhs.m_id == rhs.m_id; 
    }
    friend bool operator!=(const Entity& lhs, const Entity& rhs) 
    { 
        return lhs.m_id != rhs.m_id; 
    }

private:
    // Private constructor callable only by the friend event register  
    explicit Entity(EntityId entity) : m_id(entity) { }
    
private:
    EntityId m_id;
};

} // namespace cndt

#endif
