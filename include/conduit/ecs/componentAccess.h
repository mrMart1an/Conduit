#ifndef CNDT_ECS_COMPONENT_ACCESS_H
#define CNDT_ECS_COMPONENT_ACCESS_H

#include "conduit/ecs/entity.h"

namespace cndt {

namespace internal {
template<typename CompType>
class ComponentBuffer;
} // namespace cndt::internal

// Bundle the key with the corresponding reference to a component 
template <typename CompType>
class ComponentAccess {
    friend class internal::ComponentBuffer<CompType>;
    
private:
    // Private constructor used by the friend class 
    ComponentAccess(Entity entity, CompType &component) 
        : entity(entity), component(component) {};  

public:
    Entity entity;
    CompType &component;
};

} // namespace cndt

#endif
