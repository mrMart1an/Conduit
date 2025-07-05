#ifndef CNDT_ASSERT_H
#define CNDT_ASSERT_H

#include "conduit/internal/assert.h"

namespace cndt {

// Conduit assert macro
#define CNDT_ASSERT(cond)   \
    cndt::internal::cndt_assert(cond, #cond, __FILE__, __func__, __LINE__);

// Conduit debug assert macro
#ifndef NDEBUG
    #define CNDT_DEBUG_ASSERT(cond)   \
        cndt::internal::cndt_assert(cond, #cond, __FILE__, __func__, __LINE__);
#else
    #define CNDT_DEBUG_ASSERT(cond)
#endif

} // namespace cndt::internal

#endif
