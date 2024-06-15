#include "conduit/ecs/commandBuffer.h"
#include "conduit/ecs/world.h"

namespace cndt {

// Append a delete entity commands
void ECSCmdBuffer::deleteEntity(Entity entity)
{
    m_commands.emplace_back(
        [=](World* world) {
            world->deleteEntity(entity);
        }
    );
}
 
// Execute all the commands for the given world
void ECSCmdBuffer::runCommands(World *world_p)
{
    for (auto& cmd : m_commands) {
        cmd.executeCommand(world_p);
    }

    m_commands.clear();
}

} // namespace cndt


