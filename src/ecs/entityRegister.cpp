#include "conduit/defines.h"

#include "conduit/ecs/entity.h"

#include "conduit/internal/ecs/entityRegister.h"


namespace cndt::internal {

constexpr usize entity_free_default_size = 20;

EntityRegister::EntityRegister() :
    m_last_entity_id(0)
{ }

Entity EntityRegister::newEntity() 
{
    return Entity(m_last_entity_id++);
}

void EntityRegister::deleteEntity(Entity entity) { }

}

