#include <gtest/gtest.h>
#include <optional>

#include "conduit/internal/ecs/componentBuffer.h"
#include "conduit/ecs/componentAccess.h"
#include "conduit/ecs/entity.h"
#include "conduit/ecs/world.h"

using namespace cndt;
using namespace cndt::internal;

// Override the conduit main function at link time
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

struct CompTest {
    CompTest() : x(0),y(0),z(0) {}; 
    CompTest(int x, int y, int z) : x(x),y(y),z(z) {}; 

    int x, y, z;  
};

TEST(component_register_test, component_test) {
    World world;
    ComponentBuffer<CompTest> test_buffer;

    Entity e1 = world.newEntity();
    Entity e2 = world.newEntity();

    test_buffer.attachComponent(e1);
    test_buffer.attachComponent(e2, 2, 2, 2);

    CompTest test1 = test_buffer.getComponent(e1)->component;
    CompTest test2 = test_buffer.getComponent(e2)->component;
    
    ASSERT_EQ(test1.x, 0);
    ASSERT_EQ(test2.x, 2);
}
