#ifndef CNDT_ECS_WORLD_H
#define CNDT_ECS_WORLD_H

// Credits to the Bevy game engine for inspiring this ECS implementation
// https://github.com/bevyengine/bevy

#include "conduit/ecs/entity.h"

#include "conduit/internal/ecs/entityRegister.h"

namespace cndt::ecs {

// Store the ECS data for one scene
class World {
public:
    World();

private:
    internal::EntityRegister m_entity_register;
};

} // namespace cndt

#endif
