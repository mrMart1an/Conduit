#ifndef CNDT_EVENTS_H
#define CNDT_EVENTS_H

#include "conduit/defines.h"

namespace cndt {

// Key event 
struct KeyEvent { 
    u16 key_code;
    u16 scancode;
    u16 mods;
};

struct WindowResize { 
    u32 width;
    u32 height;
};

} // namespace cndt

#endif
