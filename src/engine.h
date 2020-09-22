#pragma once
#include "core.h"

#define ENGINE_UPDATE(name) void name(void)
typedef ENGINE_UPDATE(EngineUpdateFunc);
ENGINE_UPDATE(EngineUpdate__Undefined)
{
    return;
}