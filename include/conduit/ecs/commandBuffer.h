#ifndef CNDT_ECS_CMD_BUFFER_H
#define CNDT_ECS_CMD_BUFFER_H

#include "conduit/ecs/entity.h"
#include "conduit/ecs/world.h"

#include <functional>
#include <vector>

namespace cndt {

// Store entity component system commands
class ECSCmdBuffer {
    friend class World;
    
public:
    // Store a single ECS command
    struct Command {
        Command(std::function<void(World*)> cmd) : m_command(cmd) { }
        
        // Run the stored command
        void executeCommand(World *world_p) { m_command(world_p); };

    private:
        std::function<void(World*)> m_command;
    };
    
public:
    // Append a delete entity commands
    void deleteEntity(Entity entity);
    
    // Add a component to the buffer using the component constructor
    template <typename CompType, typename... Args>
    void attachComponent(
        Entity entity, Args... args
    );    
    
    // Copy the given component to the buffer
    template <typename CompType>
    void attachComponent(
        Entity entity,
        CompType &component
    );
    
    // Remove the component from the buffer
    template <typename CompType>
    void detachComponent(Entity entity); 
        
private:
    // Execute all the commands for the given world
    void runCommands(World *world_p);

private:
    // Store a list of commands
    std::vector<Command> m_commands;
};

// Add a component to the buffer using the component constructor
template <typename CompType, typename... Args>
void ECSCmdBuffer::attachComponent(
    Entity entity, Args... args
) {
    m_commands.emplace_back(
        [=](World* world) {
            world->attachComponent<CompType>(entity, args...);
        }
    );
}

// Copy the given component to the buffer
template <typename CompType>
void ECSCmdBuffer::attachComponent(
    Entity entity,
    CompType &component
) {
    m_commands.emplace_back(
        [=](World* world) {
            world->attachComponent<CompType>(entity, component);
        }
    );
}

// Remove the component from the buffer
template <typename CompType>
void ECSCmdBuffer::detachComponent(Entity entity) 
{
    m_commands.emplace_back(
        [=](World* world) {
            world->detachComponent<CompType>(entity);
        }
    );
}

} // namespace cndt

#endif
