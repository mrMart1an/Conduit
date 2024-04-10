#ifndef CNDT_ASSERT_H
#define CNDT_ASSERT_H

#include "conduit/defines.h"

#include <string_view>

// Conduit assert macro
#ifndef NDEBUG
    #define CNDT_ASSERT(cond)   \
        cndt::internal::assert(cond, #cond, __FILE__, __LINE__);
#else
    #define CNDT_ASSERT(cond, msg)
#endif

namespace cndt::internal {

// Conduit assert function declaration
void assert(
    bool cond,
    std::string_view cond_str,
    std::string_view file,
    i32 line
);

} // namespace cndt::internal

#endif
