#ifndef CNDT_TIME_H
#define CNDT_TIME_H

#include "conduit/defines.h"

#include <chrono>

namespace cndt::time {

/*
 *
 *      Clock definition
 *
 * */

// Return the epoch time in seconds, milliseconds and nanoseconds
class Clock {
    using ChronoClock = std::chrono::high_resolution_clock;
    
    using Duration = std::chrono::duration<f64>;
    using ms = std::chrono::milliseconds;
    using ns = std::chrono::nanoseconds;

public:
    Clock();

    // Return the current epoch time in seconds
    f64 now() const;
    
    // Return the current epoch time in milliseconds
    u64 nowMs() const;
    
    // Return the current epoch time in nanoseconds
    u64 nowNs() const;

private:
    ChronoClock m_clock;
};

/*
 *
 *      Stop watch definition
 *
 * */

// Return time elapsed since its creation and the time
// elapse since the last called to the delta function
class StopWatch {
    using ChronoClock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<ChronoClock>;
    
    using Duration = std::chrono::duration<f64>;
    using ns = std::chrono::nanoseconds;
    
public:
    StopWatch();

    // Reset the stop watch elapsed and delta time
    void reset(); 

    // Return the elapsed since the stop watch 
    // reset or creation time in nanoseconds
    f64 elapsed() const;
    
    // Return the elapsed since the stop watch 
    // reset or creation time in seconds
    u64 elapsedNs() const; 
    
    // Return the elapsed since the stop watch 
    // reset or creation time in seconds
    f64 delta();
    
    // Return the elapsed since the stop watch 
    // reset or creation time in nanoseconds
    u64 deltaNs();

private:
    ChronoClock m_clock;

    TimePoint m_begin;
    TimePoint m_last_delta;
};

} // namespace cndt::time

#endif
