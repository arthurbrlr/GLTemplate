#pragma once

#define global_var static
#define internal static
#define local_storage static

#define assert(expression) if ( !(expression) ) { *(int*)0 = 0; }

#define KiloBytes(x) ( (x) * 1024)
#define MegaBytes(x) ( KiloBytes(x) * 1024 )
#define GigaBytes(x) ( MegaBytes(x) * 1024 )

#define ARRAY_COUNT(array) ( sizeof(array) / sizeof((array)[0]) )
#define ARRAY_COPY(source, target) for (int i = 0; i < ARRAY_COUNT(source); i++) { target[i] = source[i]; }
#define ARRAY_SET(array, value) for (int i = 0; i < ARRAY_COUNT(source); i++) { array[i] = (value); }

#ifndef _WIN64
    #include <stdint.h>
#else
    #include <cstdint>
#endif

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef int bool32;

struct str8 {
    const char* string;
    u32 size;
};

struct v2i {
    union {
        i32 x;
        i32 width;
    };
    union {
        i32 y;
        i32 height;
    };
};

struct v2f {
    union {
        f32 x;
        f32 width;
    };
    union {
        f32 y;
        f32 height;
    };
};

internal void ConcatString(i64 firstCount, const char* firstStr,
                           i64 secCount, const char* secStr,
                           i64 destCount, char* destStr)
{
    for(int i = 0; i < firstCount; i++) {
        *destStr++ = *firstStr++;
    }

    for(int i = 0; i < secCount; i++)
    {
        *destStr++ = *secStr++;
    }
    *destStr++ = 0;
}

internal int GetCStringLength(const char* str)
{
    int size = 0;
    while (str[size] != '\0') {
        size++;
    }
    return size;
}

inline u32 SafeTruncateU64toU32(u64 size)
{
    assert(size <= 0xFFFFFFFF);
    return (u32)size;
}