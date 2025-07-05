#ifndef CNDT_ECS_SYSTEM_BUILDER_H
#define CNDT_ECS_SYSTEM_BUILDER_H

namespace cndt {

class SystemBuilder {

public:
    template <typename... Args>
    void system(Args...);


private:
    // Store the system function with all ofdsf 

};

} // namespace cndt

#endif
