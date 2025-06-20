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

public:
    using EntityIndex = u32;
    using GenerationId = u32;
 
private:
    // Private constructor callable only by the friend entity register  
    Entity(
        EntityIndex entity,
        GenerationId generation
    ) :
        m_index(entity),
        m_generation(generation)
    { }
   
public:
    // Return the entity index
    EntityIndex index() const { return m_index; }
    // Return the generation id
    EntityIndex gen() const { return m_generation; }

    friend bool operator==(const Entity& lhs, const Entity& rhs) 
    { 
        if (lhs.m_generation == rhs.m_generation)
            return lhs.m_index == rhs.m_index; 
        else
            return false;
    }
    friend bool operator!=(const Entity& lhs, const Entity& rhs) 
    { 
        if (lhs.m_generation == rhs.m_generation)
            return lhs.m_index != rhs.m_index; 
        else
            return true;
    }
    
    friend bool operator<(const Entity& lhs, const Entity& rhs) 
    {
        if (lhs.m_generation == rhs.m_generation)
            return lhs.m_index < rhs.m_index;
        else
            return lhs.m_generation < rhs.m_generation;
    }
    friend bool operator>(const Entity& lhs, const Entity& rhs) 
    {
        if (lhs.m_generation == rhs.m_generation)
            return lhs.m_index > rhs.m_index;
        else
            return lhs.m_generation > rhs.m_generation;
    }
    
private:
    EntityIndex m_index;
    GenerationId m_generation;
};

} // namespace cndt

#endif
