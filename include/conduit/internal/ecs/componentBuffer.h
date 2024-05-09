#ifndef CNDT_ECS_COMPONENT_BUFFER_H
#define CNDT_ECS_COMPONENT_BUFFER_H

#include "conduit/logging.h"

#include "conduit/ecs/entity.h"
#include "conduit/ecs/componentAccess.h"

#include <optional>
#include <map>
#include <shared_mutex>

namespace cndt::internal {

// Generic base Component Buffer class
class ComponentBufferBase {
public:
    ComponentBufferBase() = default;
    virtual ~ComponentBufferBase() = default;
};

// Store all the component 
template <typename CompType>
class ComponentBuffer : public ComponentBufferBase {
public:
    ComponentBuffer() = default;
    ~ComponentBuffer() = default;

    // Add a component to the buffer using the component constructor
    template <typename... Args>
    void attachComponent(Entity entity, Args... args);

    // Get a component for the given entity 
    // return an optional value storing nullopt if the component for 
    // the given entity was not found
    std::optional<ComponentAccess<CompType>> getComponent(Entity entity);

    // Remove the component from the buffer
    void detachComponent(Entity entity);

private:
    // Check if a component is stored for the given entity
    bool componentExist(Entity entity);

private:
    std::shared_mutex m_mutex;

    std::map<Entity, CompType> m_component_buffer;
};

/*
 *
 *      Component buffer template implementation
 *
 * */

// Check if a component is stored for the given entity
template <typename CompType>
bool ComponentBuffer<CompType>::componentExist(Entity entity) 
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    return (m_component_buffer.find(entity) != m_component_buffer.end());
} 

// Add a component to the buffer using the component constructor
template <typename CompType>
template <typename... Args>
void ComponentBuffer<CompType>::attachComponent(
    Entity entity, Args... args
) {
    if (!componentExist(entity)) {
        std::lock_guard<std::shared_mutex> lock(m_mutex);
        
        m_component_buffer[entity] = CompType(args...);    
    } else {
        log::core::warn(
            "ComponentBuffer::addComponent -> component already exist"
        );
    }
}

// Get a component for the given entity
template <typename CompType>
std::optional<ComponentAccess<CompType>> 
ComponentBuffer<CompType>::getComponent(Entity entity)
{
    if (componentExist(entity)) {
        std::shared_lock<std::shared_mutex> lock(m_mutex);

        ComponentAccess<CompType> component(
            entity, 
            m_component_buffer[entity]
        );

        return component;
        
    } else {
        return std::nullopt;
    }
}

// Remove the component from the buffer
template <typename CompType>
void ComponentBuffer<CompType>::detachComponent(Entity entity) 
{
    std::lock_guard<std::shared_mutex> lock(m_mutex);
    
    m_component_buffer.erase(entity);
}

} // namespace cndt::internal

#endif
