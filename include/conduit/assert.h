#ifndef CNDT_ASSERT_H
#define CNDT_ASSERT_H

#include "conduit/internal/assert.h"

namespace cndt {

// Conduit assert macro
#define CNDT_ASSERT(cond)   \
    cndt::internal::cndt_assert<#cond, __FILE__, __func__, __LINE__>(cond);

// Conduit debug assert macro
#ifndef NDEBUG
    #define CNDT_DEBUG_ASSERT(cond)   \
        cndt::internal::cndt_assert<#cond, __FILE__, __func__, __LINE__>(cond);
#else
    #define CNDT_DEBUG_ASSERT(cond)
#endif

} // namespace cndt::internal

#endif
