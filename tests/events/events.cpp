#include <gtest/gtest.h>

#include "conduit/defines.h"

#include "conduit/events/eventBus.h"
#include "conduit/events/eventWriter.h"
#include "conduit/events/eventReader.h"

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
 *      Define test functions
 *
 * */

TEST(events_test, reader_count) {
    EventBus bus;

    EventWriter writer = bus.getEventWriter();

    auto empty_reader = bus.getEventReader<EmptyEvent>();    
    auto int_reader = bus.getEventReader<IntEvent>();    
    auto array_reader = bus.getEventReader<ArrayEvent>();    
    
    ASSERT_EQ(empty_reader.availableEvent(), 0);
    ASSERT_EQ(int_reader.availableEvent(), 0);
    ASSERT_EQ(array_reader.availableEvent(), 0);

    writer.send<EmptyEvent>(EmptyEvent());
    ASSERT_EQ(empty_reader.availableEvent(), 1);
    writer.send<EmptyEvent>(EmptyEvent());
    ASSERT_EQ(empty_reader.availableEvent(), 2);
    writer.send<EmptyEvent>(EmptyEvent());
    
    ASSERT_EQ(empty_reader.availableEvent(), 3);
    ASSERT_EQ(int_reader.availableEvent(), 0);
    ASSERT_EQ(array_reader.availableEvent(), 0);

    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<IntEvent>(IntEvent(test_int));
    
    ASSERT_EQ(empty_reader.availableEvent(), 3);
    ASSERT_EQ(int_reader.availableEvent(), 3);
    ASSERT_EQ(array_reader.availableEvent(), 0);

    writer.send<ArrayEvent>(ArrayEvent(test_array));
    writer.send<ArrayEvent>(ArrayEvent(test_array));
    writer.send<ArrayEvent>(ArrayEvent(test_array));
    
    ASSERT_EQ(empty_reader.availableEvent(), 3);
    ASSERT_EQ(int_reader.availableEvent(), 3);
    ASSERT_EQ(array_reader.availableEvent(), 3);

    bus.update();

    writer.send<EmptyEvent>(EmptyEvent());
    writer.send<EmptyEvent>(EmptyEvent());
    writer.send<EmptyEvent>(EmptyEvent());
    
    ASSERT_EQ(empty_reader.availableEvent(), 6);
    ASSERT_EQ(int_reader.availableEvent(), 3);
    ASSERT_EQ(array_reader.availableEvent(), 3);

    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<IntEvent>(IntEvent(test_int));
    writer.send<IntEvent>(IntEvent(test_int));
    
    ASSERT_EQ(empty_reader.availableEvent(), 6);
    ASSERT_EQ(int_reader.availableEvent(), 6);
    ASSERT_EQ(array_reader.availableEvent(), 3);

    writer.send<ArrayEvent>(ArrayEvent(test_array));
    writer.send<ArrayEvent>(ArrayEvent(test_array));
    writer.send<ArrayEvent>(ArrayEvent(test_array));
    
    ASSERT_EQ(empty_reader.availableEvent(), 6);
    ASSERT_EQ(int_reader.availableEvent(), 6);
    ASSERT_EQ(array_reader.availableEvent(), 6);

    bus.update();

    ASSERT_EQ(empty_reader.availableEvent(), 3);
    ASSERT_EQ(int_reader.availableEvent(), 3);
    ASSERT_EQ(array_reader.availableEvent(), 3);
    
    bus.update();

    ASSERT_EQ(empty_reader.availableEvent(), 0);
    ASSERT_EQ(int_reader.availableEvent(), 0);
    ASSERT_EQ(array_reader.availableEvent(), 0);

    // Read count test

    for (int i = 0; i < 10; i++) {
        writer.send<EmptyEvent>(EmptyEvent());
        writer.send<IntEvent>(IntEvent(test_int));
        writer.send<ArrayEvent>(ArrayEvent(test_array));
        
        ASSERT_EQ(empty_reader.availableEvent(), i + 1);
        ASSERT_EQ(int_reader.availableEvent(), i + 1);
        ASSERT_EQ(array_reader.availableEvent(), i + 1);
    }
    
    for (int i = 0; i < 5; i++) {
        empty_reader.begin();
        int_reader.begin();
        array_reader.begin();
        
        ASSERT_EQ(empty_reader.availableEvent(), 9 - i);
        ASSERT_EQ(int_reader.availableEvent(), 9 - i);
        ASSERT_EQ(array_reader.availableEvent(), 9 - i);
    }
    
    bus.update();

    for (int i = 0; i < 10; i++) {
        writer.send<EmptyEvent>(EmptyEvent());
        writer.send<IntEvent>(IntEvent(test_int));
        writer.send<ArrayEvent>(ArrayEvent(test_array));
        
        ASSERT_EQ(empty_reader.availableEvent(), i + 6);
        ASSERT_EQ(int_reader.availableEvent(), i + 6);
        ASSERT_EQ(array_reader.availableEvent(), i + 6);
    }
    
    for (int i = 0; i < 5; i++) {
        empty_reader.begin();
        int_reader.begin();
        array_reader.begin();
        
        ASSERT_EQ(empty_reader.availableEvent(), 14 - i);
        ASSERT_EQ(int_reader.availableEvent(), 14 - i);
        ASSERT_EQ(array_reader.availableEvent(), 14 - i);
    }

    bus.update();
    
    ASSERT_EQ(empty_reader.availableEvent(), 10);
    ASSERT_EQ(int_reader.availableEvent(), 10);
    ASSERT_EQ(array_reader.availableEvent(), 10);
    
    for (int i = 0; i < 5; i++) {
        empty_reader.begin();
        int_reader.begin();
        array_reader.begin();
        
        ASSERT_EQ(empty_reader.availableEvent(), 9 - i);
        ASSERT_EQ(int_reader.availableEvent(), 9 - i);
        ASSERT_EQ(array_reader.availableEvent(), 9 - i);
    }
    
    bus.update();
    
    ASSERT_EQ(empty_reader.availableEvent(), 0);
    ASSERT_EQ(int_reader.availableEvent(), 0);
    ASSERT_EQ(array_reader.availableEvent(), 0);
}

