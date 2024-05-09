#include "conduit/defines.h"
#include "conduit/logging.h"

#include "conduit/ecs/entity.h"

#include "conduit/internal/ecs/entityRegister.h"

#include <algorithm>

namespace cndt::internal {

constexpr usize entity_free_default_size = 20;

EntityRegister::EntityRegister() :
    m_last_entity_id(0),
    m_free_entity_list()
{ 
    m_free_entity_list.reserve(entity_free_default_size);
}

Entity EntityRegister::newEntity() 
{
    // Return an entity from the free list if it's not empty
    // otherwise increment the last entity counter
    if (m_free_entity_list.size() > 0) {
        Entity entity_out = m_free_entity_list.back(); 
        m_free_entity_list.pop_back();
        
        return entity_out;
    } else {
        return Entity(m_last_entity_id++);
    }
}

void EntityRegister::deleteEntity(Entity entity) 
{
    // Check if the entity was already freed
    bool entity_freed = std::find(
        m_free_entity_list.begin(),
        m_free_entity_list.end(),
        entity
    ) != m_free_entity_list.end();

    // Check if the entity exist in the register
    if (entity.id() > m_last_entity_id || entity_freed) {
        log::core::warn(
            "EntityRegister::deleteEntity -> entity doesn't exist"
        );
        
        return;
    }

    // If the given entity is the last, decrement the last entity index
    if (entity.id() != m_last_entity_id) {
        m_free_entity_list.push_back(entity);
    } else {
        m_last_entity_id--;        
    }
}

}

