#include "conduit/internal/core/deleteQueue.h"

#include "conduit/application.h"

namespace cndt {

/*
 *
 *      Engine initialization and shutdown 
 * 
 * */

// Base application constructor
Application::Application() :
    m_ecs_world(),
    m_delete_queue()
{ };

// Base application deconstructor
Application::~Application() {
    m_delete_queue.callDeleter();
};

} // namespace cndt
