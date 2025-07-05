#include "conduit/ecs/entity.h"

#include "conduit/internal/ecs/entityRegister.h"

namespace cndt::ecs::internal {

EntityRegister::EntityRegister() :
    m_largest_entity_id(0)
{ 
    // Preallocate space in the free list vector
    m_free_list.reserve(entity_free_list_start_size);
}

Entity EntityRegister::newEntity() 
{
    if (m_free_list.empty()) {
        // If the free list is empty return a brand new entity and 
        // increment the largest entity id counter
        m_largest_entity_id++;
        return Entity(m_largest_entity_id, 0);
    } else {
        // If an entity is in the free list increase it's generation,
        // remove it from the free list and return it
        Entity entity = m_free_list.back();
        m_free_list.pop_back();

        entity.m_generation++;
        
        return entity;
    }
}

void EntityRegister::deleteEntity(Entity entity) 
{ 
    m_free_list.push_back(entity);
}

} // namespace cndt::ecs::internal

