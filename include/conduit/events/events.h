#ifndef CNDT_EVENTS_H
#define CNDT_EVENTS_H

#include "conduit/defines.h"

namespace cndt {

/*
 *
 *      Keyboard events
 *
 * */

struct KeyPressEvent { 
    u32 key_code;
    u32 mods;
};

struct KeyRepeatEvent { 
    u32 key_code;
    u32 mods;
};

struct KeyReleaseEvent { 
    u32 key_code;
    u32 mods;
};

/*
 *
 *      Mouse events
 *
 * */

struct MouseKeyPressEvent { 
    u32 button_code;
    u32 mods;
};

struct MouseKeyReleaseEvent { 
    u32 button_code;
    u32 mods;
};

struct MouseScrollEvent {
    f64 x_scroll;
    f64 y_scroll;
};
    
struct MousePositionEvent {
    f64 x_pos;
    f64 y_pos;
};

/*
 *
 *      Window events
 *
 * */

struct WindowCloseEvent { };

struct WindowFocusGainEvent { };
struct WindowFocusLostEvent { };

struct WindowResizeEvent {
    i32 width;
    i32 height;
};

struct WindowMoveEvent {
    i32 x_pos;
    i32 y_pos;
};

} // namespace cndt

#endif
