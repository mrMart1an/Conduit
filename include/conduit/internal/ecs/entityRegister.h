#ifndef CNDT_ECS_ENTITY_REG_H
#define CNDT_ECS_ENTITY_REG_H

#include "conduit/ecs/entity.h"

namespace cndt::internal {

// Keep track of the created entity in the world class
class EntityRegister {
public:
    EntityRegister();
    
    Entity newEntity();

    void deleteEntity(Entity entity);

private:
    // Last assigned entity
    Entity::EntityId m_last_entity_id;
};

} // namespace cndt::internal

#endif
