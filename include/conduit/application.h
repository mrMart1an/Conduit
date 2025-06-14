#ifndef CNDT_APPLICATION_H
#define CNDT_APPLICATION_H

#include "conduit/ecs/world.h"
#include "conduit/renderer/renderer.h"
#include "conduit/window/window.h"
#include "conduit/internal/core/deleteQueue.h"

namespace cndt {

class DeleteQueue;

// Base application class 
class Application {
    friend class AppRunner;

public:
    Application();
    virtual ~Application();

protected:
    // Return a string as the application name
    virtual std::string appName() const { return "Conduit application"; };

protected:
    // ECS world
    ecs::World m_ecs_world;

private:
    // Application deleter queue
    internal::DeleteQueue m_delete_queue;
};

} // namespace cndt

#endif
