#include <gtest/gtest.h>

#include "conduit/ecs/world.h"
#include "conduit/ecs/commandBuffer.h"

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
struct CompThird {
    CompThird() : a(0) {}; 
    CompThird(int a) : a(a) {}; 
    
    int a;
};

TEST(query_read_test, world_test) {
    World world;

    std::vector<Entity> entities;
    
    // Create entities
    for (int i = 0; i < 20; i++) {
        Entity e = world.newEntity();
        entities.push_back(e);
    }

    // Create components
    for (int i = 0; i < 15; i++) {
        Entity e = entities.at(i);
        world.attachComponent<CompFirst>(e, 10 + i);
    }
    for (int i = 5; i < 20; i++) {
        Entity e = entities.at(i);
        world.attachComponent<CompSecond>(e, 20 + i);
    }
    for (int i = 0; i < 10; i++) {
        Entity e = entities.at(i*2);
        world.attachComponent<CompThird>(e, 30 + i*2);
    }

    // Single query test
    {
        auto query_first = world.createQuery<CompFirst>();

        ASSERT_EQ(15, query_first.size());
        for (auto element : query_first) {
            ASSERT_EQ(element.entity().id(), element.get<CompFirst>().x - 10);
        }
        
        auto query_second = world.createQuery<CompSecond>();

        ASSERT_EQ(15, query_second.size());
        for (auto element : query_second) {
            ASSERT_EQ(element.entity().id(), element.get<CompSecond>().r - 20);
        }

        auto query_third = world.createQuery<CompThird>();

        ASSERT_EQ(10, query_third.size());
        for (auto element : query_third) {
            ASSERT_EQ(element.entity().id(), element.get<CompThird>().a - 30);
        }
    }   
    
    // Double query test
    {
        auto query = world.createQuery<CompSecond, CompFirst>();
    
        ASSERT_EQ(10, query.size());
        for (auto element : query) {
            ASSERT_EQ(element.entity().id(), element.get<CompFirst>().x - 10);
            ASSERT_EQ(element.entity().id(), element.get<CompSecond>().r - 20);
        }
    }
    {
        auto query = world.createQuery<CompFirst, CompSecond>();

        ASSERT_EQ(10, query.size());
        for (auto element : query) {
            ASSERT_EQ(element.entity().id(), element.get<CompFirst>().x - 10);
            ASSERT_EQ(element.entity().id(), element.get<CompSecond>().r - 20);
        }
    }
    {
        auto query = world.createQuery<CompFirst, CompThird>();

        ASSERT_EQ(8, query.size());
        for (auto element : query) {
            ASSERT_EQ(element.entity().id(), element.get<CompFirst>().x - 10);
            ASSERT_EQ(element.entity().id(), element.get<CompThird>().a - 30);
        }
    }
    {
        auto query = world.createQuery<CompThird, CompSecond>();

        ASSERT_EQ(7, query.size());
        for (auto element : query) {
            ASSERT_EQ(element.entity().id(), element.get<CompSecond>().r - 20);
            ASSERT_EQ(element.entity().id(), element.get<CompThird>().a - 30);
        }
    }

    // Triple query test
    {
        auto query = world.createQuery<CompThird, CompSecond, CompFirst>();
    
        ASSERT_EQ(5, query.size());
        for (auto element : query) {
            ASSERT_EQ(element.entity().id(), element.get<CompFirst>().x - 10);
            ASSERT_EQ(element.entity().id(), element.get<CompSecond>().r - 20);
            ASSERT_EQ(element.entity().id(), element.get<CompThird>().a - 30);
        }
    }
    {
        auto query = world.createQuery<CompFirst, CompSecond, CompThird>();
    
        ASSERT_EQ(5, query.size());
        for (auto element : query) {
            ASSERT_EQ(element.entity().id(), element.get<CompFirst>().x - 10);
            ASSERT_EQ(element.entity().id(), element.get<CompSecond>().r - 20);
            ASSERT_EQ(element.entity().id(), element.get<CompThird>().a - 30);
        }
    }
    {
        auto query = world.createQuery<CompFirst, CompThird, CompSecond>();
    
        ASSERT_EQ(5, query.size());
        for (auto element : query) {
            ASSERT_EQ(element.entity().id(), element.get<CompFirst>().x - 10);
            ASSERT_EQ(element.entity().id(), element.get<CompSecond>().r - 20);
            ASSERT_EQ(element.entity().id(), element.get<CompThird>().a - 30);
        }
    }
    {
        auto query = world.createQuery<CompSecond, CompThird, CompFirst>();
    
        ASSERT_EQ(5, query.size());
        for (auto element : query) {
            ASSERT_EQ(element.entity().id(), element.get<CompFirst>().x - 10);
            ASSERT_EQ(element.entity().id(), element.get<CompSecond>().r - 20);
            ASSERT_EQ(element.entity().id(), element.get<CompThird>().a - 30);
        }
    }

    // Array operator test
    {
        auto query = world.createQuery<CompSecond, CompThird, CompFirst>();
    
        ASSERT_EQ(5, query.size());
        for (int i = 0; i < 5; i++) {
            auto element = query[i];

            ASSERT_EQ(element.entity().id(), element.get<CompFirst>().x - 10);
            ASSERT_EQ(element.entity().id(), element.get<CompSecond>().r - 20);
            ASSERT_EQ(element.entity().id(), element.get<CompThird>().a - 30);
        }
    }
}

TEST(query_write_test, world_test) {
    World world;

    std::vector<Entity> entities;
    
    // Create entities
    for (int i = 0; i < 20; i++) {
        Entity e = world.newEntity();
        entities.push_back(e);
    }

    // Create components
    for (int i = 0; i < 15; i++) {
        Entity e = entities.at(i);
        world.attachComponent<CompFirst>(e, 1);
    }
    for (int i = 5; i < 20; i++) {
        Entity e = entities.at(i);
        world.attachComponent<CompSecond>(e, 2);
    }

    // Read and write to the components
    {
        auto query = world.createQuery<CompFirst, CompSecond>();

        ASSERT_EQ(10, query.size());
        for (auto element : query) {
            ASSERT_EQ(1, element.get<CompFirst>().x);
            ASSERT_EQ(2, element.get<CompSecond>().r);

            element.get<CompFirst>().x = element.entity().id() + 10;
            element.get<CompSecond>().r = element.entity().id() + 20;
            
            ASSERT_EQ(element.entity().id() + 10, element.get<CompFirst>().x);
            ASSERT_EQ(element.entity().id() + 20, element.get<CompSecond>().r);
        }
    }
    
    // Create a different query and test again
    {
        auto query = world.createQuery<CompSecond, CompFirst>();

        ASSERT_EQ(10, query.size());
        for (auto element : query) {
            ASSERT_EQ(element.entity().id() + 10, element.get<CompFirst>().x);
            ASSERT_EQ(element.entity().id() + 20, element.get<CompSecond>().r);
        }
    }
} 


TEST(query_update_test, world_test) {
    World world;

    std::vector<Entity> entities;
    
    // Create entities
    for (int i = 0; i < 20; i++) {
        Entity e = world.newEntity();
        entities.push_back(e);
    }
    
    // First update
    for (int i = 0; i < 5; i++) {
        Entity e = entities.at(i);
        world.attachComponent<CompFirst>(e, 1);
    }
    for (int i = 0; i < 10; i++) {
        Entity e = entities.at(i);
        world.attachComponent<CompSecond>(e, 2);
    }

    {
        auto query = world.createQuery<CompSecond, CompFirst>();

        ASSERT_EQ(5, query.size());
        for (auto element : query) {
            ASSERT_EQ(1, element.get<CompFirst>().x);
            ASSERT_EQ(2, element.get<CompSecond>().r);
        }
    }
    
    // Second update
    for (int i = 5; i < 15; i++) {
        Entity e = entities.at(i);
        world.attachComponent<CompFirst>(e, 1);
    }
    
    {
        auto query = world.createQuery<CompSecond, CompFirst>();

        ASSERT_EQ(10, query.size());
        for (auto element : query) {
            ASSERT_EQ(1, element.get<CompFirst>().x);
            ASSERT_EQ(2, element.get<CompSecond>().r);
        }
    }

    // Third update
    for (int i = 10; i < 20; i++) {
        Entity e = entities.at(i);
        world.attachComponent<CompSecond>(e, 2);
    }
    
    {
        auto query = world.createQuery<CompSecond, CompFirst>();

        ASSERT_EQ(15, query.size());
        for (auto element : query) {
            ASSERT_EQ(1, element.get<CompFirst>().x);
            ASSERT_EQ(2, element.get<CompSecond>().r);
        }
    }

    // Fourth update
    for (int i = 0; i < 5; i++) {
        Entity e = entities.at(i);
        world.detachComponent<CompFirst>(e);
    }
    for (int i = 5; i < 10; i++) {
        Entity e = entities.at(i);
        world.detachComponent<CompSecond>(e);
    }
    
    {
        auto query = world.createQuery<CompSecond, CompFirst>();

        ASSERT_EQ(5, query.size());
        for (auto element : query) {
            ASSERT_EQ(1, element.get<CompFirst>().x);
            ASSERT_EQ(2, element.get<CompSecond>().r);
        }
    }
}

TEST(cmd_buffer_test, world_test) {
    World world;
    ECSCmdBuffer cmd_buffer;

    std::vector<Entity> entities;
    
    for (int i = 0; i < 20; i++) {
        Entity e = world.newEntity();
        entities.push_back(e);
    }

    // Test entity delete
    cmd_buffer.deleteEntity(entities.at(0));
    world.executeCmdBuffer(cmd_buffer);
    ASSERT_EQ(0, world.newEntity().id());

    // Attach components test
    for (int i = 0; i < 10; i++) {
        cmd_buffer.attachComponent<CompFirst>(entities.at(i), i);
    }

    world.executeCmdBuffer(cmd_buffer);

    // Detach components
    {
        auto query = world.createQuery<CompFirst>();
        
        ASSERT_EQ(10, query.size());
        for (auto element : query) {
            if (element.get<CompFirst>().x < 5) {
                cmd_buffer.detachComponent<CompFirst>(element.entity());
            }
        }
    }

    world.executeCmdBuffer(cmd_buffer);
    
    {
        auto query = world.createQuery<CompFirst>();
        ASSERT_EQ(5, query.size());
    }
}
