#include "conduit/defines.h"

#include "conduit/events/eventBus.h"
#include "conduit/events/eventWriter.h"

namespace cndt {

// Default starting capacity of the buffers storage vectors
constexpr usize default_buffers_size = 30;

// Construct the event bus
EventBus::EventBus() : m_type_id_last(0) {
    // Reserve space for the default events
    m_event_buffers.reserve(default_buffers_size);
}
EventBus::~EventBus() { }

// Return an event writer
EventWriter EventBus::GetEventWriter() {
    return EventWriter(this);
}

// Swap the event buffers and run all the callbacks
void EventBus::Update() {
    // Update the event buffers
    for (auto &buffer : m_event_buffers) {
        buffer->Update();
    }
}

} // namespace cndt
