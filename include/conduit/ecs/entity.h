#ifndef CNDT_ECS_ENTITY_H
#define CNDT_ECS_ENTITY_H

#include "conduit/defines.h"

namespace cndt::ecs {

namespace internal {

class EntityRegister;

} // namespace cndt::ecs::internal

// ECS entity type definition 
class Entity {
    friend class internal::EntityRegister;

private:
    using EntityId = u32;
    using GenerationId = u32;
 
private:
    // Private constructor callable only by the friend entity register  
    Entity(
        EntityId entity,
        GenerationId generation
    ) :
        m_id(entity),
        m_generation(generation)
    { }
   
public:
    friend bool operator==(const Entity& lhs, const Entity& rhs) 
    { 
        if (lhs.m_generation == rhs.m_generation)
            return lhs.m_id == rhs.m_id; 
        else
            return false;
    }
    friend bool operator!=(const Entity& lhs, const Entity& rhs) 
    { 
        if (lhs.m_generation == rhs.m_generation)
            return lhs.m_id != rhs.m_id; 
        else
            return true;
    }
    
    friend bool operator<(const Entity& lhs, const Entity& rhs) 
    {
        if (lhs.m_generation == rhs.m_generation)
            return lhs.m_id < rhs.m_id;
        else
            return lhs.m_generation < rhs.m_generation;
    }
    friend bool operator>(const Entity& lhs, const Entity& rhs) 
    {
        if (lhs.m_generation == rhs.m_generation)
            return lhs.m_id > rhs.m_id;
        else
            return lhs.m_generation > rhs.m_generation;
    }
    
private:
    EntityId m_id;
    GenerationId m_generation;
};

} // namespace cndt

#endif
