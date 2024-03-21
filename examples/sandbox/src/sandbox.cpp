#include "conduit/logging.h"

using namespace cndt::log;

int main(void) {

    app::trace("test {} {} {}", 3, 3.14, "conduit");
    app::debug("test {} {} {}", 3, 3.14, "conduit");
    app::info("test {} {} {}", 3, 3.14, "conduit");
    app::warn("test {} {} {}", 3, 3.14, "conduit");
    app::error("test {} {} {}", 3, 3.14, "conduit");
    app::fatal("test {} {} {}", 3, 3.14, "conduit");

    app::trace("string only test");
    app::debug("string only test");
    app::info("string only test");
    app::warn("string only test");
    app::error("string only test");
    app::fatal("string only test");

    return 0;
}

