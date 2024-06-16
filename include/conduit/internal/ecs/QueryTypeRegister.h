#ifndef CNDT_ECS_QUERY_TYPE_REG_H
#define CNDT_ECS_QUERY_TYPE_REG_H

#include "conduit/defines.h"

#include <atomic>

namespace cndt::internal {

// Track Query types ids
class QueryTypeRegister {
public:
    using TypeId = u64;

    // Return an unique id for the given query type
    template <typename... CompTypes>
    static TypeId getTypeId() {
        static std::atomic<TypeId> id(getNextTypeId());
        return id;
    }

private:
    // Make the class non constructable
    QueryTypeRegister() = delete;

    // Return an unique id to initialize the query type id 
    // static variable of the query struct
    static TypeId getNextTypeId() {
        static std::atomic<TypeId> m_last_id(0);
        return m_last_id++;
    }       
};

} // namespace cndt::internal

#endif

