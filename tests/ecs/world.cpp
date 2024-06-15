#include <gtest/gtest.h>

#include "conduit/ecs/world.h"
#include "conduit/ecs/commandBuffer.h"
#include "conduit/logging.h"

#include <vector>

using namespace cndt;

// Override the conduit main function at link time
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

struct CompFirst {
    CompFirst() : x(0) {}; 
    CompFirst(int x) : x(x) {}; 
    
    int x;  
};
struct CompSecond {
    CompSecond() : r(0) {}; 
    CompSecond(int r) : r(r) {}; 
    
    int r;  
};

TEST(query_test, world_test) {
    World world;

    std::vector<Entity> entities;
    
    for (int i = 0; i < 15; i++) {
        Entity e = world.newEntity();
        entities.push_back(e);
    }

    // Create components
    for (int i = 0; i < 10; i++) {
        Entity e = entities.at(i);
        world.attachComponent<CompFirst>(e, 1);
    }
    for (int i = 5; i < 15; i++) {
        Entity e = entities.at(i);
        world.attachComponent<CompSecond>(e, 2);
    }

    auto query_storage = world.createQuery<CompFirst, CompSecond>();
    auto query = query_storage.createQuery();
}

TEST(cmd_buffer_test, world_test) {
    World world;
    ECSCmdBuffer cmd_buffer;

    std::vector<Entity> entities;
    
    for (int i = 0; i < 15; i++) {
        Entity e = world.newEntity();
        entities.push_back(e);
    }

    cmd_buffer.deleteEntity(entities.at(0));
    world.executeCmdBuffer(cmd_buffer);

    ASSERT_EQ(0, world.newEntity().id());

    // Attach components test
    for (int i = 0; i < 15; i++) {
        cmd_buffer.attachComponent<CompFirst>(entities.at(i));
    }

    world.executeCmdBuffer(cmd_buffer);
}
