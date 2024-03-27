#ifndef CNDT_ASSERT_H
#define CNDT_ASSERT_H

#include "conduit/defines.h"

#include <memory>

namespace cndt {

// Base application class 
class Application {
public:
    Application();
    virtual ~Application();

    // Application update function, 
    // called once per frame to update the scene
    virtual void Update(f32 delta_time) = 0;

public:
    // Start the main application loop
    // this function shouldn't be called by the client
    void StartMainLoop();
};

// Return an instance of an user defined application object
// to be implemented by the client
std::unique_ptr<Application> getEntryClass();
    
} // namespace cndt
 
// Macro to generate the get entry class function
#define CNDT_ENTRY_CLASS(Type)                                  \
    std::unique_ptr<cndt::Application> cndt::getEntryClass() {  \
        return std::make_unique<Type>();                        \
    }                                                            

#endif
