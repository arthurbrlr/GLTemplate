#pragma once
#include "core.h"

struct debug_file {
    u32 size;
    void* content;
};

#define DEBUG_PLATFORM_IO_FREEFILEMEMORY(name) void name(void* fileMemory)
typedef DEBUG_PLATFORM_IO_FREEFILEMEMORY(DEBUG_PlatformIOFreeFileMemoryFunc);

#define DEBUG_PLATFORM_IO_READENTIREFILE(name) debug_file name(const char* filepath)
typedef DEBUG_PLATFORM_IO_READENTIREFILE(DEBUG_PlatformIOReadEntireFileFunc);

#define DEBUG_PLATFORM_IO_WRITEENTIREFILE(name) bool32 name(const char* filepath, debug_file* file)
typedef DEBUG_PLATFORM_IO_WRITEENTIREFILE(DEBUG_PlatformIOWriteEntireFileFunc);