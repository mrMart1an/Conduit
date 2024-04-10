#include "conduit/defines.h"
#include "conduit/logging.h"

#include "conduit/assert.h"

namespace cndt::internal {

// Conduit assert function declaration
void assert(
    bool cond,
    std::string_view cond_str,
    std::string_view file,
    i32 line
) {
    if (!cond) {
        log::core::fatal(
            "Assert fail: \"{}\"; file: {}; line: {}",
            cond_str, file, line
        );

        CNDT_ABORT();
    }
}

} // namespace cndt::internal
