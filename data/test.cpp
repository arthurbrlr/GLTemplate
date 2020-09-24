#include "core/platform.h"
#include "core/filesystem.h"
#include "engine.h"

#include "core/log.cpp"
#include "core/events.cpp"

internal void DisplayMessages()
{
    LogInfo("Hello from the Engine!\n");
    LogMsg("This is an info\n");
    LogWarn("This is a warn\n");
    LogErr("This is an error\n");
}

#define max_menu_labels 4
struct engine_menu {
    const char* menuLabels[max_menu_labels] = {
        "Play",
        "Options",
        "Exit",
        "Credits"
    };
};
global_var engine_menu engineMenu;

struct engine_state {
    i32 counter;
    i8 currentMenuPosition;
};

extern "C" ENGINE_UPDATE(EngineUpdateAndRender)
{
    engine_state* state = (engine_state*)engineMemory->permanentContainer;

    if ( !engineMemory->isInitialized ) {
        engineMemory->isInitialized = true;
        state->counter = 480;

        debug_file file = engineMemory->readfile(__FILE__);
        if ( file.content ) {
            engineMemory->writefile("test.cpp", &file);
            engineMemory->freefile(file.content);
        }

        LogInfo("Engine memory initialized\n");
    }

    if ( state->counter-- < 0 ) {
        state->counter = 480;
        //DisplayMessages();
    }

    if ( OnControllerButtonDown(0, controller_button_DPAD_DOWN) ) {
        state->currentMenuPosition++;
        if ( state->currentMenuPosition == max_menu_labels ) state->currentMenuPosition = max_menu_labels - 1;
        LogMsg("%s\n", engineMenu.menuLabels[state->currentMenuPosition]);
    }

    if ( OnControllerButtonDown(0, controller_button_DPAD_UP) ) {
        state->currentMenuPosition--;
        if ( state->currentMenuPosition < 0 ) state->currentMenuPosition = 0;
        LogMsg("%s\n", engineMenu.menuLabels[state->currentMenuPosition]);
    }
}

extern "C" ENGINE_SETPLATFORM(EngineSetPlatform)
{
    enginePlatform = pointer;
    if ( enginePlatform ) {
        LogInfo("Platform pointer set successfully\n");
    }
}