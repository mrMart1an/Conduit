#include "conduit/defines.h"

#include "conduit/time.h"

#include <chrono>

namespace cndt::time {

/*
 *
 *      Clock declaration
 *
 * */

Clock::Clock() : m_clock() { }

// Return the current epoch time in seconds
f64 Clock::now() const  
{
    Duration time = m_clock.now().time_since_epoch();   
    return time.count();
}

// Return the current epoch time in milliseconds
u64 Clock::nowMs() const 
{
    Duration time = m_clock.now().time_since_epoch();   
    ms timeMs = std::chrono::duration_cast<ms>(time);
    
    return timeMs.count();
}

// Return the current epoch time in nanoseconds
u64 Clock::nowNs() const 
{
    Duration time = m_clock.now().time_since_epoch();   
    ns timeNs = std::chrono::duration_cast<ns>(time);
    
    return timeNs.count();
}

/*
 *
 *      Stop watch declaration
 *
 * */

// Return time elapsed since its creation and the time
// elapse since the last called to the delta function
StopWatch::StopWatch() : m_clock() { reset(); }

// Reset the stop watch elapsed and delta time
void StopWatch::reset() 
{
    m_begin = m_clock.now();
    m_last_delta = m_clock.now();
}

// Return the elapsed since the stop watch 
// reset or creation time in nanoseconds
f64 StopWatch::elapsed() const 
{
    Duration duration = m_clock.now() - m_begin;
    
    return duration.count();
}

// Return the elapsed since the stop watch 
// reset or creation time in seconds
u64 StopWatch::elapsedNs() const 
{
    Duration duration = m_clock.now() - m_begin;
    ns durationNs = std::chrono::duration_cast<ns>(duration);
    
    return durationNs.count();
}

// Return the elapsed since the stop watch 
// reset or creation time in seconds
f64 StopWatch::delta()
{
    Duration duration = m_clock.now() - m_last_delta;
    m_last_delta = m_clock.now();
    
    return duration.count();
}

// Return the elapsed since the stop watch 
// reset or creation time in nanoseconds
u64 StopWatch::deltaNs()
{
    Duration duration = m_clock.now() - m_last_delta;
    ns durationNs = std::chrono::duration_cast<ns>(duration);
    m_last_delta = m_clock.now();
    
    return durationNs.count();
}

} // namespace cndt::time
