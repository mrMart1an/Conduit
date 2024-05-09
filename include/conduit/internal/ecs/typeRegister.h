#ifndef CNDT_ECS_COMPONENT_TYPE_REG_H
#define CNDT_ECS_COMPONENT_TYPE_REG_H

#include "conduit/defines.h"

#include <atomic>

namespace cndt::internal {

// Track components type and component types ids
class ComponentTypeRegister {
public:
    using TypeId = u64;

    // Return an unique id for the given component type
    template <typename CompType>
    static TypeId getTypeId() {
        static std::atomic<TypeId> id(getNextTypeId());
        return id;
    }

private:
    // Make the class non constructable
    ComponentTypeRegister() = delete;

    // Return an unique id to initialize the component type id 
    // static variable of the component struct
    static TypeId getNextTypeId() {
        static std::atomic<TypeId> m_last_id(0);
        return m_last_id++;
    }       
};

} // namespace cndt::internal

#endif

