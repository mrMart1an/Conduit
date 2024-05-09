
#ifndef CNDT_ECS_COMPONENT_REG_H
#define CNDT_ECS_COMPONENT_REG_H

#include "conduit/ecs/entity.h"
#include "conduit/internal/ecs/typeRegister.h"
#include "conduit/internal/ecs/componentBuffer.h"

#include <map>
#include <memory>
#include <shared_mutex>

namespace cndt::internal {

class ComponentRegister {
    
public:
    // Attach component to the entity,
    // construct the component with the provided arguments.
    // Only one component per type can be assigned to an entity
    template <typename CompType, typename... Args>
    void attachComponent(Entity entity, Args... args);

    // Detach a component from the given entity
    template <typename CompType>
    void detachComponent(Entity entity);

private:
    // Add the component type to the register if it doesn't already exist
    template<class CompType>
    void addComponetType();
    
private:
    std::shared_mutex m_mutex;
    
    // Store generic unique pointer to the components buffers
    using TypeId = ComponentTypeRegister::TypeId;
    using ComponentBufferPtr = std::unique_ptr<ComponentBufferBase>;
    
    std::map<TypeId, ComponentBufferPtr> m_component_buffers;
};

// Attach component to the entity,
// construct the component with the provided arguments.
// Only one component per type can be assigned to an entity
template <typename CompType, typename... Args>
void ComponentRegister::attachComponent(Entity entity, Args... args)
{
    // Create the buffer if it doesn't already exist
    addComponetType<CompType>();
    
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    // Get a reference to the component buffer and add the component to it
    auto type_id = ComponentTypeRegister::getTypeId<CompType>();
    ComponentBuffer<CompType>& buffer = m_component_buffers[type_id];

    buffer.attachComponent(entity, args...);
}

// Detach a component from the given entity
template <typename CompType>
void ComponentRegister::detachComponent(Entity entity)
{
    // Create the buffer if it doesn't already exist
    addComponetType<CompType>();
    
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    // Get a reference to the component buffer 
    // and remove the component from it
    auto type_id = ComponentTypeRegister::getTypeId<CompType>();
    ComponentBuffer<CompType>& buffer = m_component_buffers[type_id];

    buffer.detachComponent(entity);
}

// Add the component type to the register if it doesn't already exist
template<class CompType>
void ComponentRegister::addComponetType() 
{
    // Get the unique component type id
    auto type_id = ComponentTypeRegister::getTypeId<CompType>();
    
    // Check if the component buffer already exist
    bool type_exist = false;
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        if (m_component_buffers.find(type_id) != m_component_buffers.end())
            type_exist = true;
    }

    // If the component buffer doesn't exist lock the mutex and 
    // add the component to the register
    if (!type_exist) {
        std::lock_guard<std::shared_mutex> lock(m_mutex);
        
        auto buffer = std::make_unique<ComponentBuffer<CompType>>();
        m_component_buffers[type_id] = std::move(buffer);
    }
}

}

#endif
