
#ifndef CNDT_ECS_COMPONENT_REG_H
#define CNDT_ECS_COMPONENT_REG_H

#include "conduit/ecs/entity.h"
#include "conduit/internal/ecs/ComponentTypeRegister.h"
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
    
    // Attach component to the entity,
    // Copy the given component to the buffer
    // Only one component per type can be assigned to an entity
    template <typename CompType>
    void attachComponent(Entity entity, CompType &component);
    
    // Detach a component from the given entity
    template <typename CompType>
    void detachComponent(Entity entity);

    // Detach all the components from the given entity
    void detachAllComponets(Entity entity);

    // Get an component buffer for the specific type
    // if the component doesn't exist create it
    template<class CompType>
    std::weak_ptr<ComponentBuffer<CompType>> getComponentBuffer();

private:
    // Add the component type to the register if it doesn't already exist
    template<class CompType>
    void addComponetType();
    
private:
    std::shared_mutex m_mutex;
    
    // Store generic unique pointer to the components buffers
    using TypeId = ComponentTypeRegister::TypeId;
    using ComponentBufferPtr = std::shared_ptr<ComponentBufferBase>;
    
    std::map<TypeId, ComponentBufferPtr> m_component_buffers;
};

// Get an component buffer for the specific type
// if the component doesn't exist create it
template<class CompType>
std::weak_ptr<ComponentBuffer<CompType>> 
ComponentRegister::getComponentBuffer()
{
    // Create the buffer if it doesn't already exist
    addComponetType<CompType>();

    // Cast the buffer shared pointer to a weak pointer
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    auto type_id = ComponentTypeRegister::getTypeId<CompType>();
    
    return std::static_pointer_cast<ComponentBuffer<CompType>>(
        m_component_buffers[type_id]
    );
}

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
    
    std::shared_ptr<ComponentBuffer<CompType>> buffer =
        std::static_pointer_cast<ComponentBuffer<CompType>>(
            m_component_buffers[type_id]
        );

    buffer->attachComponent(entity, args...);
}
    
// Attach component to the entity,
// Copy the given component to the buffer
// Only one component per type can be assigned to an entity
template <typename CompType>
void ComponentRegister::attachComponent(Entity entity, CompType &component)
{
    // Create the buffer if it doesn't already exist
    addComponetType<CompType>();
    
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    // Get a reference to the component buffer and add the component to it
    auto type_id = ComponentTypeRegister::getTypeId<CompType>();
    
    std::shared_ptr<ComponentBuffer<CompType>> buffer =
        std::static_pointer_cast<ComponentBuffer<CompType>>(
            m_component_buffers[type_id]
        );

    buffer->attachComponent(entity, component);
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
    
    std::shared_ptr<ComponentBuffer<CompType>> buffer =
        std::static_pointer_cast<ComponentBuffer<CompType>>(
            m_component_buffers[type_id]
        );

    buffer->detachComponent(entity);
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
        
        type_exist =
            m_component_buffers.find(type_id) != m_component_buffers.end();
    }

    // If the component buffer doesn't exist lock the mutex and 
    // add the component to the register
    if (!type_exist) {
        std::lock_guard<std::shared_mutex> lock(m_mutex);
        
        auto buffer = std::make_shared<ComponentBuffer<CompType>>();
        m_component_buffers[type_id] = std::move(buffer);
    }
}

}

#endif
