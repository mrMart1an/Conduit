#ifndef CNDT_ECS_WORLD_H
#define CNDT_ECS_WORLD_H

#include "conduit/ecs/entity.h"

#include "conduit/internal/ecs/entityRegister.h"

namespace cndt {

// Store the ECS data for one scene
class World {
public:
    World();

    // Create a new empty entity 
    Entity newEntity();

    // Delete an entity and it's associate components  
    void deleteEntity(Entity entity);
    
private:
    internal::EntityRegister m_entity_register;
};

} // namespace cndt

#endif
