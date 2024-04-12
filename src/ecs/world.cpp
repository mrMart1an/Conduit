#include "conduit/ecs/world.h"

namespace cndt {

World::World(): m_entity_register() { }

Entity World::newEntity() 
{
    return m_entity_register.newEntity();    
}

void World::deleteEntity(Entity entity) 
{
    m_entity_register.deleteEntity(entity);    
}

} // namespace cndt
