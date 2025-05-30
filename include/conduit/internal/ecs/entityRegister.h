#ifndef CNDT_ECS_ENTITY_REG_H
#define CNDT_ECS_ENTITY_REG_H

#include <vector>

#include "conduit/ecs/entity.h"

namespace cndt::ecs::internal {

// Keep track of the created entity in the world class
class EntityRegister {
private:
    // The default preallocated size of the entity free list 
    static constexpr usize entity_free_list_start_size = 1000;

public:
    EntityRegister();
    
    // Generate a new unique entity
    Entity newEntity();

    // Delete an entity and add it to the free list
    void deleteEntity(Entity entity);

private:
    // Store the current largest entity id assigned by the register
    Entity::EntityId m_largest_entity_id;

    // Free list of entity ready to be reassigned  
    std::vector<Entity> m_free_list;
};

} // namespace cndt::internal

#endif
