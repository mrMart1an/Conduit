#include "conduit/ecs/world.h"
#include "conduit/ecs/commandBuffer.h"

namespace cndt {

World::World(): m_entity_register() { }

Entity World::newEntity() 
{
    return m_entity_register.newEntity();    
}

void World::deleteEntity(Entity entity) 
{
    // Detach all the components from the entity 
    m_component_register.detachAllComponets(entity);

    m_entity_register.deleteEntity(entity);    
}

// Execute the commands from the given commands buffer
void World::executeCmdBuffer(ECSCmdBuffer& cmd_buffer)
{
    cmd_buffer.runCommands(this);
}

} // namespace cndt
