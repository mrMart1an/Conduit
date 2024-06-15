#include "conduit/internal/ecs/componentRegister.h"

namespace cndt::internal {

// Detach all the components from the given entity
void ComponentRegister::detachAllComponets(Entity entity)
{
    for (auto& buf : m_component_buffers) {
        buf.second->detachComponent(entity);
    }   
}

} // namespace cndt::internal
