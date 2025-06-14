#ifndef CNDT_ASSERT_INTERNAL_H
#define CNDT_ASSERT_INTERNAL_H

#include "conduit/defines.h"

namespace cndt::internal {

// Conduit assert function declaration
template <typename T>
void cndt_assert(
    T cond,
    const char* cond_str,
    const char* file,
    const char* func,
    i32 line
) {
    if (!cond) {
        log::core::fatal(
            "Assert fail: \"{}\"; file: {}; function: {}; line: {}",
            cond_str, file, func, line
        );

        cndt::abort();
    }
};

} // namespace cndt::internal

#endif
