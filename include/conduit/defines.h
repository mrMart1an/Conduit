#ifndef CNDT_DEFINES_H
#define CNDT_DEFINES_H

#include <cstddef>
#include <cstdint>
#include <exception>
#include <string>
#include <string_view>

/*
 *
 *      Essential macro definitions
 *
 * */

// Immediately halt execution of the engine
#define CNDT_ABORT() std::abort()

// Compile time static assertion 
#define STATIC_ASSERT(cond, error_msg) static_assert(cond, error_msg)

/*
 *
 *      Conduit types definitions
 *
 * */

namespace cndt {

// 1 bytes (8 bits) unsigned integer type
typedef uint8_t   u8;
// 2 bytes (16 bits) unsigned integer type
typedef uint16_t  u16;
// 4 bytes (32 bits) unsigned integer type
typedef uint32_t  u32;
// 8 bytes (64 bits) unsigned integer type
typedef uint64_t  u64;

// Unsigned integer type capable of storing an array index
typedef size_t    usize;

// 1 bytes (8 bits) signed integer type
typedef int8_t    i8;
// 2 bytes (16 bits) signed integer type
typedef int16_t   i16;
// 4 bytes (32 bits) signed integer type
typedef int32_t   i32;
// 8 bytes (64 bits) signed integer type
typedef int64_t   i64;

// 4 bytes (32 bits) signed integer type
typedef float     f32;
// 8 bytes (64 bits) float point type
typedef double    f64;

// Signed integer type capable of storing a pointer
typedef intptr_t  intptr;
// Unsigned integer type capable of storing a pointer
typedef uintptr_t uintptr;

// Conduit sized boolean type
 
// 1 bytes (8 bits) boolean type
typedef uint8_t   b8;
// 4 bytes (32 bits) boolean type
typedef uint32_t  b32;

// Type size checks
STATIC_ASSERT(sizeof(u8)  == 1, "sizeof(u18) != 1");
STATIC_ASSERT(sizeof(u16) == 2, "sizeof(u16) != 2");
STATIC_ASSERT(sizeof(u32) == 4, "sizeof(u32) != 4");
STATIC_ASSERT(sizeof(u64) == 8, "sizeof(u64) != 8");

STATIC_ASSERT(sizeof(i8)  == 1, "sizeof(i18) != 1");
STATIC_ASSERT(sizeof(i16) == 2, "sizeof(i16) != 2");
STATIC_ASSERT(sizeof(i32) == 4, "sizeof(i32) != 4");
STATIC_ASSERT(sizeof(i64) == 8, "sizeof(i64) != 8");

STATIC_ASSERT(sizeof(f32) == 4, "sizeof(f32) != 4");
STATIC_ASSERT(sizeof(f64) == 8, "sizeof(f64) != 8");

STATIC_ASSERT(sizeof(b8)  == 1, "sizeof(b8) != 1");
STATIC_ASSERT(sizeof(b32) == 4, "sizeof(b32) != 4");

/*
 *
 *      Conduit default exception type
 *
 * */

// Conduit generic exception
class Exception : public std::exception {
public:
    Exception(std::string_view message) : m_message(message) { }
    Exception() : m_message("Conduit exception") { }
    
    virtual const char* what() const throw() 
    {
        return m_message.c_str();   
    }
    
private:
    // Store the exception message
    std::string m_message;
};

}

#endif
