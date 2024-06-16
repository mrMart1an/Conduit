#ifndef CNDT_ECS_WORLD_H
#define CNDT_ECS_WORLD_H

#include "conduit/ecs/entity.h"

#include "conduit/internal/ecs/componentRegister.h"
#include "conduit/internal/ecs/entityRegister.h"
#include "conduit/internal/ecs/queryRegister.h"

namespace cndt {

class ECSCmdBuffer;

// Store the ECS data for one scene
class World {
public:
    World();

    // Create a new empty entity 
    Entity newEntity();

    // Delete an entity and it's associate components  
    void deleteEntity(Entity entity);
    
    // Attach component to the entity,
    // construct the component with the provided arguments.
    // Only one component per type can be assigned to an entity
    template <typename CompType, typename... Args>
    void attachComponent(Entity entity, Args... args);
    
    // Attach component to the entity,
    // Copy the given component to the buffer
    // Only one component per type can be assigned to an entity
    template <typename CompType>
    void attachComponent(Entity entity, CompType &component);
    
    // Detach a component from the given entity
    template <typename CompType>
    void detachComponent(Entity entity);

    // Create a query for the given arguments list
    // the query will store a list of entity witch are associated 
    // with all of the given components
    template<typename... ComponentsTypes>
    Query<ComponentsTypes...> createQuery();

    // Execute the commands from the given commands buffer
    void executeCmdBuffer(ECSCmdBuffer& cmd_buffer);

private:
    internal::EntityRegister m_entity_register;
    internal::ComponentRegister m_component_register;
    internal::QueryRegister m_query_register;
};

// Create a query for the given arguments list
// the query will store a list of entity witch are associated 
// with all of the given components
template<typename... ComponentsTypes>
Query<ComponentsTypes...> World::createQuery()
{
    return m_query_register.getQuery<ComponentsTypes...>(
        m_component_register
    );
}

// Attach component to the entity,
// construct the component with the provided arguments.
// Only one component per type can be assigned to an entity
template <typename CompType, typename... Args>
void World::attachComponent(Entity entity, Args... args)
{
    m_component_register.attachComponent<CompType>(entity, args...);
}

// Attach component to the entity,
// Copy the given component to the buffer
// Only one component per type can be assigned to an entity
template <typename CompType>
void World::attachComponent(Entity entity, CompType &component)
{
    m_component_register.attachComponent<CompType>(entity, component);
}

// Detach a component from the given entity
template <typename CompType>
void World::detachComponent(Entity entity)
{
    m_component_register.detachComponent<CompType>(entity);
}

} // namespace cndt

#endif
