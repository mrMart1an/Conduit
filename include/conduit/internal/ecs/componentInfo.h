#ifndef CNDT_ECS_COMPONENT_INFO_H
#define CNDT_ECS_COMPONENT_INFO_H

#include "conduit/internal/ecs/tick.h"

namespace cndt::ecs::internal {

// Store data associated with an individual component
struct ComponentInfo {
    Tick m_added_tick;
    Tick m_changed_tick;
};

} // namespace cndt::ecs::internal

#endif
