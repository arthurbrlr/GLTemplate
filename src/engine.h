#pragma once
#include "core.h"

struct engine_memory {
    bool32 isInitialized;
    u64 permanentContainerSize;
    void* permanentContainer;
    u64 transientContainerSize;
    void* transientContainer;
};

#define ENGINE_UPDATE(name) void name(engine_memory* engineMemory)
typedef ENGINE_UPDATE(EngineUpdateFunc);
ENGINE_UPDATE(EngineUpdate__Undefined)
{
    return;
}

struct platform;

#define ENGINE_SETPLATFORM(name) void name(platform* pointer)
typedef ENGINE_SETPLATFORM(EngineSetPlatformFunc);
ENGINE_SETPLATFORM(EngineSetPlatform__Undefined)
{
}