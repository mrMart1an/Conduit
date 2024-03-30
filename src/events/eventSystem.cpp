#include "conduit/events/eventBus.h"
#include "conduit/events/eventWriter.h"

#include <memory>

namespace cndt {

// Construct the event bus
EventBus::EventBus() :
    m_event_register(std::make_shared<internal::EventRegister>()) 
{ }

EventBus::~EventBus() { }

// Return an event writer
EventWriter EventBus::GetEventWriter() {
    return EventWriter(m_event_register);
}

// Swap the event buffers and run all the callbacks
void EventBus::Update() {
    // Executing all the callbacks before swapping buffer
    // in event register update
    m_callback_register.ExecuteCallback();
    
    // Update the event register
    m_event_register->Update();
}

} // namespace cndt
