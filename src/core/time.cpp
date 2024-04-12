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

f64 Clock::now() const  
{
    Duration time = m_clock.now().time_since_epoch();   
    return time.count();
}

u64 Clock::nowMs() const 
{
    Duration time = m_clock.now().time_since_epoch();   
    ms timeMs = std::chrono::duration_cast<ms>(time);
    
    return timeMs.count();
}

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

StopWatch::StopWatch() : m_clock() { reset(); }

void StopWatch::reset() 
{
    m_last_delta = m_clock.now();
}

f64 StopWatch::elapsed() const 
{
    Duration duration = m_clock.now() - m_last_delta;
    
    return duration.count();
}

u64 StopWatch::elapsedNs() const 
{
    Duration duration = m_clock.now() - m_last_delta;
    ns durationNs = std::chrono::duration_cast<ns>(duration);
    
    return durationNs.count();
}

f64 StopWatch::delta()
{
    Duration duration = m_clock.now() - m_last_delta;
    m_last_delta = m_clock.now();
    
    return duration.count();
}

u64 StopWatch::deltaNs()
{
    Duration duration = m_clock.now() - m_last_delta;
    ns durationNs = std::chrono::duration_cast<ns>(duration);
    m_last_delta = m_clock.now();
    
    return durationNs.count();
}

} // namespace cndt::time
