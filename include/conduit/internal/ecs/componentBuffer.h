#ifndef CNDT_ECS_COMPONENT_BUFFER_H
#define CNDT_ECS_COMPONENT_BUFFER_H

#include "conduit/logging.h"

#include "conduit/ecs/entity.h"

#include <algorithm>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace cndt::internal {

// Generic base Component Buffer class
class ComponentBufferBase {
public:
    ComponentBufferBase() = default;
    virtual ~ComponentBufferBase() = default;

    // Remove the component from the buffer
    virtual void detachComponent(Entity entity) = 0;
};

// Store all the component 
template <typename CompType>
class ComponentBuffer : public ComponentBufferBase {
private:
    using EntityIterator = std::vector<Entity>::iterator;
    using ComponentIterator = std::vector<CompType>::iterator;
    
public:
    ComponentBuffer() = default;
    ~ComponentBuffer() = default;

    // Add a component to the buffer using the component constructor
    template <typename... Args>
    void attachComponent(Entity entity, Args... args);
    
    // Copy the given component to the buffer
    void attachComponent(Entity entity, CompType &component);

    // Remove the component from the buffer
    void detachComponent(Entity entity) override;

    // Get a reference to the entity vector 
    std::vector<Entity>& entityVector() { return m_entity_buffer; }

    // Get a reference to the component vector 
    std::vector<CompType>& componentVector() { return m_component_buffer; }
    
    // Return the buffer version
    u64 version() const { return m_version; }

    // Lock the mutex and return a unique lock 
    std::shared_lock<std::shared_mutex> lock() 
    {
        return std::shared_lock(m_mutex);
    }
    
private:
    // Return the upper bound iterator for the given entity 
    EntityIterator componentUpperBound(Entity entity);
    
    // Take the upper bound for a given entity and return true 
    // if the element already exist
    bool componentExist(EntityIterator upper_bound, Entity entity);

private:
    std::shared_mutex m_mutex;
    
    // Vector of entity, always stored in crescent order
    std::vector<Entity> m_entity_buffer;

    // Vector of components, the corresponding entity is
    // in the entity vector at the same index of the component
    std::vector<CompType> m_component_buffer;

    // Buffer version
    u64 m_version;
};

/*
 *
 *      Component buffer template implementation
 *
 * */

// Check if a component is stored for the given entity
template <typename CompType>
ComponentBuffer<CompType>::EntityIterator
ComponentBuffer<CompType>::componentUpperBound(Entity entity) 
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    // Binary search to check if the element exist
    auto upper_bound = std::upper_bound(
        m_entity_buffer.begin(),
        m_entity_buffer.end(),
        entity
    );
    
    return upper_bound;
} 
 
// Take the upper bound for a given entity and return true 
// if the element already exist
template <typename CompType>
bool ComponentBuffer<CompType>::componentExist(
    EntityIterator upper_bound,
    Entity entity
) {
    if (m_entity_buffer.empty())
        return false;

    if ((upper_bound - 1)->id() == entity.id())
        return true;
    
    return false;
}

// Add a component to the buffer using the component constructor
template <typename CompType>
template <typename... Args>
void ComponentBuffer<CompType>::attachComponent(
    Entity entity, Args... args
) {
    EntityIterator upper_bound = componentUpperBound(entity);

    if (!componentExist(upper_bound, entity)) {
        std::lock_guard<std::shared_mutex> lock(m_mutex);
        
        
        CompType component(args...);
        usize index = upper_bound - m_entity_buffer.begin();
        auto upper_bound_comp = m_component_buffer.begin() + index;
        
        m_entity_buffer.insert(upper_bound, entity);
        m_component_buffer.insert(upper_bound_comp, component);    
        
        m_version += 1;
    } else {
        log::core::warn(
            "ComponentBuffer::addComponent -> component already exist"
        );
    }
}

// Copy the given component to the buffer
template <typename CompType>
void ComponentBuffer<CompType>::attachComponent(
    Entity entity,
    CompType &component
) {
    EntityIterator upper_bound = componentUpperBound(entity);

    if (!componentExist(upper_bound, entity)) {
        std::lock_guard<std::shared_mutex> lock(m_mutex);
        
        
        usize index = upper_bound - m_entity_buffer.begin();
        auto upper_bound_comp = m_component_buffer.begin() + index;
        
        m_entity_buffer.insert(upper_bound, entity);
        m_component_buffer.insert(upper_bound_comp, component);      
        
        m_version += 1;
    } else {
        log::core::warn(
            "ComponentBuffer::addComponent -> component already exist"
        );
    }
}

// Remove the component from the buffer
template <typename CompType>
void ComponentBuffer<CompType>::detachComponent(Entity entity) 
{
    EntityIterator upper_bound = componentUpperBound(entity);
    
    if (componentExist(upper_bound, entity)) {
        std::lock_guard<std::shared_mutex> lock(m_mutex);
        
        
        usize index = upper_bound - m_entity_buffer.begin();
        auto upper_bound_comp = m_component_buffer.begin() + index;
        
        m_entity_buffer.erase(upper_bound - 1);
        m_component_buffer.erase(upper_bound_comp - 1);
        
        m_version += 1;
    }
}

} // namespace cndt::internal

#endif
