#include <gtest/gtest.h>

#include "conduit/internal/ecs/componentBuffer.h"
#include "conduit/ecs/entity.h"
#include "conduit/ecs/world.h"
#include "conduit/internal/ecs/componentRegister.h"

#include <vector>

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

TEST(component_buffer_test, component_test) {
    World world;
    ComponentBuffer<CompTest> test_construct;
    ComponentBuffer<CompTest> test_copy;

    std::vector<Entity> allocated_entity;

    for (int i = 0; i < 10; i++) {
        Entity e = world.newEntity();
        allocated_entity.push_back(e);
        
        test_construct.attachComponent(e, i, i*2, i*3);
        
        CompTest test_comp(i*3, i*2, i);
        test_copy.attachComponent(e, test_comp);
    }

    int test_i = 0;
    for (const auto& comp : test_construct.componentVector()) {
        ASSERT_EQ(test_i, comp.x);
        ASSERT_EQ(test_i*2, comp.y);
        ASSERT_EQ(test_i*3, comp.z);
        
        test_i += 1;
    }
    
    test_i = 0;
    for (const auto& comp : test_copy.componentVector()) {
        ASSERT_EQ(test_i*3, comp.x);
        ASSERT_EQ(test_i*2, comp.y);
        ASSERT_EQ(test_i, comp.z);
        
        test_i += 1;
    }

    // Detach test
    for (int i = 0; i < 5; i++) {
        Entity e = allocated_entity.at(i * 2);
        test_construct.detachComponent(e);
    }

    test_i = 1;
    for (const auto& comp : test_construct.componentVector()) {
        ASSERT_EQ(test_i, comp.x);
        ASSERT_EQ(test_i*2, comp.y);
        ASSERT_EQ(test_i*3, comp.z);
        
        test_i += 2;
    }

    // Unsorted attach test
    for (int i = 0; i < 5; i++) {
        Entity e = allocated_entity.at(i * 2);
        
        test_construct.attachComponent(e, i*2, i*4, i*6);
    }

    test_i = 0;
    for (const auto& comp : test_construct.componentVector()) {
        ASSERT_EQ(test_i, comp.x);
        ASSERT_EQ(test_i*2, comp.y);
        ASSERT_EQ(test_i*3, comp.z);
        
        test_i += 1;
    }

    // Test buffer lock
    std::shared_lock<std::shared_mutex> lock = test_construct.lock();
}

TEST(component_register_test, component_test) {
    World world;
    ComponentRegister test_register;

    std::vector<Entity> allocated_entity;

    for (int i = 0; i < 10; i++) {
        Entity e = world.newEntity();
        allocated_entity.push_back(e);
        
        test_register.attachComponent<CompTest>(e, i, i*2, i*3);
        test_register.attachComponent<int>(e, i);
    }

    auto test_comp = test_register.getComponentBuffer<CompTest>().lock();
    auto test_int = test_register.getComponentBuffer<int>().lock();

    int test_i = 0;
    for (auto& comp : test_comp->componentVector()) {
        ASSERT_EQ(test_i, comp.x);
        ASSERT_EQ(test_i*2, comp.y);
        ASSERT_EQ(test_i*3, comp.z);
        
        test_i += 1;
    } 
    test_i = 0;
    for (auto& comp : test_int->componentVector()) {
        ASSERT_EQ(test_i, comp);
        
        test_i += 1;
    } 

    // Test detach
    for (int i = 0; i < 5; i++) {
        Entity e = allocated_entity.at(i * 2);
        test_register.detachComponent<int>(e);
        test_register.detachComponent<CompTest>(e);
    }
    
    test_i = 1;
    for (const auto& comp : test_comp->componentVector()) {
        ASSERT_EQ(test_i, comp.x);
        ASSERT_EQ(test_i*2, comp.y);
        ASSERT_EQ(test_i*3, comp.z);
        
        test_i += 2;
    }
    test_i = 1;
    for (auto& comp : test_int->componentVector()) {
        ASSERT_EQ(test_i, comp);
        
        test_i += 2;
    } 
}
