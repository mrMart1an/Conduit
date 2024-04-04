#include <gtest/gtest.h>

#include "conduit/defines.h"

#include "conduit/events/eventBus.h"
#include "conduit/events/eventWriter.h"

using namespace cndt;

// Override the conduit main function at link time
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*
 *
 *      Define test value
 *
 * */

constexpr u32 test_int = 100042;
constexpr u32 test_array = 42600;

/*
 *
 *      Define event types
 *
 * */
 
struct EmptyEvent { };

struct IntEvent {
    // Storage value
    u32 value;
};

struct ArrayEvent {
    ArrayEvent(u32 value) { 
        setArray(value);
    }

    // Storage value
    u32 array[10];

    // Set all the elements in the array to the given value
    void setArray(u32 value) {
        for (u32& a_val : array) {
            a_val = value;
        }
    }
    
    // Check if all the elements in the array are equal to the given value
    bool checkArray(u32 value) const {
        for (u32 a_val : array) {
            if (a_val != value)
                return false;
        }

        return true;
    }
};

/*
 *
 *      Define callback functions
 *
 * */

static u64 empty_count = 0;
void empty_callback(const EmptyEvent*) {
    empty_count++;
}

static u64 int_count = 0;
void int_callback(const IntEvent*) {
    int_count++;
}

static u64 array_count = 0;
void array_callback(const ArrayEvent*) {
    array_count++;
}

void int_callback_check(const IntEvent* event) {
    ASSERT_EQ(event->value, test_int);
}

void array_callback_check(const ArrayEvent* event) {
    ASSERT_TRUE(event->checkArray(test_array));
}

/*
 *
 *      Define callback functions
 *
 * */

TEST(callback_test, single_update) {
    // Reset static variable
    empty_count = 0; 
    int_count = 0; 
    array_count = 0; 

    EventBus bus;

    // Create an event writer and attach the callbacks
    EventWriter writer = bus.getEventWriter();

    bus.addCallback<EmptyEvent>(empty_callback);
    bus.addCallback<IntEvent>(int_callback);
    bus.addCallback<ArrayEvent>(array_callback);
    
    bus.addCallback<IntEvent>(int_callback_check);
    bus.addCallback<ArrayEvent>(array_callback_check);
    
    // Write some events to the buffer
    writer.send<EmptyEvent>(EmptyEvent());
    writer.send<EmptyEvent>(EmptyEvent());
    writer.send<EmptyEvent>(EmptyEvent());
    
    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<IntEvent>(IntEvent(test_int));
    
    writer.send<ArrayEvent>(ArrayEvent(test_array));
    writer.send<ArrayEvent>(ArrayEvent(test_array));
    writer.send<ArrayEvent>(ArrayEvent(test_array));

    ASSERT_EQ(empty_count, 0);
    ASSERT_EQ(int_count, 0);
    ASSERT_EQ(array_count, 0);
    
    // Run the callbacks
    bus.update();

    ASSERT_EQ(empty_count, 3);
    ASSERT_EQ(int_count, 3);
    ASSERT_EQ(array_count, 3);
}

TEST(callback_test, multi_update) {
    // Reset static variable
    empty_count = 0; 
    int_count = 0; 
    array_count = 0; 

    EventBus bus;

    // Create an event writer and attach the callbacks
    EventWriter writer = bus.getEventWriter();

    bus.addCallback<EmptyEvent>(empty_callback);
    bus.addCallback<IntEvent>(int_callback);
    bus.addCallback<ArrayEvent>(array_callback);
    
    bus.addCallback<IntEvent>(int_callback_check);
    bus.addCallback<ArrayEvent>(array_callback_check);
    
    // Write some events to the buffer
    writer.send<EmptyEvent>(EmptyEvent());
    writer.send<EmptyEvent>(EmptyEvent());
    writer.send<EmptyEvent>(EmptyEvent());
    
    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<IntEvent>(IntEvent(test_int));
    
    writer.send<ArrayEvent>(ArrayEvent(test_array));
    writer.send<ArrayEvent>(ArrayEvent(test_array));
    writer.send<ArrayEvent>(ArrayEvent(test_array));

    ASSERT_EQ(empty_count, 0);
    ASSERT_EQ(int_count, 0);
    ASSERT_EQ(array_count, 0);
    
    // Run the callbacks
    bus.update();

    ASSERT_EQ(empty_count, 3);
    ASSERT_EQ(int_count, 3);
    ASSERT_EQ(array_count, 3);
    
    // Write some events to the buffer
    writer.send<EmptyEvent>(EmptyEvent());
    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<ArrayEvent>(ArrayEvent(test_array));
    
    writer.send<EmptyEvent>(EmptyEvent());
    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<ArrayEvent>(ArrayEvent(test_array));
    
    writer.send<EmptyEvent>(EmptyEvent());
    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<ArrayEvent>(ArrayEvent(test_array));
    
    writer.send<EmptyEvent>(EmptyEvent());
    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<ArrayEvent>(ArrayEvent(test_array));

    ASSERT_EQ(empty_count, 3);
    ASSERT_EQ(int_count, 3);
    ASSERT_EQ(array_count, 3);

    // Run the callbacks
    bus.update();

    ASSERT_EQ(empty_count, 7);
    ASSERT_EQ(int_count, 7);
    ASSERT_EQ(array_count, 7);
}
