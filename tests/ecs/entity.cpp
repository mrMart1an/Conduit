#include <gtest/gtest.h>

#include "conduit/ecs/entity.h"
#include "conduit/ecs/world.h"

using namespace cndt;

// Override the conduit main function at link time
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(entity_test, entity_test) {
    World world;

    Entity e1 = world.newEntity();
    Entity e2 = world.newEntity();
    Entity e3 = world.newEntity();

    ASSERT_TRUE(e1 != e2);
    ASSERT_TRUE(e2 != e3);
    ASSERT_TRUE(e1 != e3);

    world.deleteEntity(e2);
    Entity e4 = world.newEntity();
    
    ASSERT_TRUE(e2 == e4);
    
    world.deleteEntity(e1);
    Entity e5 = world.newEntity();
    
    ASSERT_TRUE(e1 == e5);
    
    ASSERT_TRUE(e5 != e4);
    ASSERT_TRUE(e4 != e3);
    ASSERT_TRUE(e5 != e3);

    // Double delete

    world.deleteEntity(e5);
    world.deleteEntity(e5);
    
    Entity e6 = world.newEntity();
    Entity e7 = world.newEntity();
    
    ASSERT_TRUE(e5 == e6);
    ASSERT_TRUE(e6 != e7);
}
