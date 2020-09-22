#include "core.h"
#include "core/log.cpp"
#include "engine.h"

extern "C" ENGINE_UPDATE(EngineUpdateAndRender)
{
    local_storage i32 counter = 60;
    if ( counter-- < 0 ) {
        LogMsg("Hello from the Engine!\n");
        counter = 60;
    }
}