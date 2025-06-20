#ifndef CNDT_ECS_COMPONENT_STORAGE_H
#define CNDT_ECS_COMPONENT_STORAGE_H

#include "conduit/internal/ecs/componentInfo.h"
#include "conduit/internal/ecs/tick.h"

#include <vector>

namespace cndt::ecs::internal {

// Store components and associated meta data in a continuous array
template <typename T>
class ComponentStorage {
private:
    static constexpr usize DEFAULT_RESERVE_SIZE = 100;

public:
    // Create a component storage buffer and reserve 
    // a default amount of space
    ComponentStorage() {
        m_components.reserve(DEFAULT_RESERVE_SIZE);
        m_infos.reserve(DEFAULT_RESERVE_SIZE);
    };
    // Create a component storage buffer and reserve
    // the given amount of space
    ComponentStorage(usize reserve) {
        m_components.reserve(reserve);
        m_infos.reserve(reserve);
    };

public:
    // Push a component to the end of the storage 
    // and create an info struct using the given add tick
    void pushBack(Tick add_tick, T& comp);

    // Construct a component to the end of the storage
    // and create an empty info struct
    template <typename... Args>
    void emplaceBack(Tick add_tick, Args& ...args);

private:
    // Component storage array
    std::vector<T> m_components;

    // Store the component info
    std::vector<ComponentInfo> m_infos;
};

// Push a component to the end of the storage 
// and create an empty info struct
template <typename T>
void ComponentStorage<T>::pushBack(Tick add_tick, T& comp)
{
    m_components.push_back(comp);
    m_infos.emplace_back(add_tick, add_tick);
}

// Construct a component to the end of the storage
// and create an empty info struct
template <typename T>
template <typename... Args>
void ComponentStorage<T>::emplaceBack(Tick add_tick, Args& ...args)
{
    m_components.emplace_back(args...);
    m_infos.emplace_back(add_tick, add_tick);
}

}

#endif
