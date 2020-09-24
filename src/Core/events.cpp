#include "events.h"
#include "platform.h"

global_var platform* enginePlatform;

internal str8 GetStringFromArray(int id, const char** array, int arraySize)
{
    str8 result;
    if ( id >= 0 && id < arraySize ) {
        result.string = array[id];
    } else {
        result.string = "Undefined";
    }
    result.size = GetCStringLength(result.string);
    return result;
}

internal str8 GetControllerButtonName(engine_controller_button button)
{
    local_storage const char* buttonNames[max_controller_buttons] = {
        #define controllerbutton(key, str) str,
        #include "controller_buttons.def"
        #undef controllerbutton
    };

    str8 result = GetStringFromArray(button, buttonNames, max_controller_buttons);
    return result;
}

internal bool32 GetControllerButton(int controllerID, engine_controller_button button)
{
    return enginePlatform->currentControllers[controllerID].buttons[button];
}

internal bool32 OnControllerButtonDown(int controllerID, engine_controller_button button)
{
    return enginePlatform->currentControllers[controllerID].buttons[button] 
    && !enginePlatform->previousControllers[controllerID].buttons[button];
}

internal bool32 OnControllerButtonUp(int controllerID, engine_controller_button button)
{
    return !enginePlatform->currentControllers[controllerID].buttons[button] 
    && enginePlatform->previousControllers[controllerID].buttons[button];
}

internal str8 GetControllerAxesName(engine_controller_axes axes)
{
    local_storage const char* axesNames[max_controller_axes] = {
        #define controlleraxes(axes, str) str,
        #include "controller_axes.def"
        #undef controlleraxes
    };

    str8 result = GetStringFromArray( (int)axes, axesNames, max_controller_axes);
    return result;
}

internal f32 GetControllerAxes(int controllerID, engine_controller_axes axes)
{
    return enginePlatform->currentControllers[controllerID].axes[axes];
}

    // NOTE(arthur): see https://www.glfw.org/docs/latest/group__keys.html for more details on key odes
internal engine_key KeyFromGLFWKeycode(int keycode)
{
    engine_key key = KEY_UNKNOWN;
    if ( keycode == -1 ) {
        key = KEY_UNKNOWN;
    } else if ( keycode >= 44 && keycode <= 57 ) { // 0-9 keys and "," and "-"  and "." and "/" keys
        key = (Keys)(keycode - 41);
    } else if ( keycode >= 65 && keycode <= 90 ) { // A-Z keys
        key = (Keys)(keycode - 46);
    } else if ( keycode == 32 ) { // "   " key
        key = KEY_SPACE;
    } else if ( keycode == 39 ) { // " ' " key
        key = KEY_APOSTROPHE;
    } else if ( keycode == 59 ) { // ";" key
        key = KEY_SEMICOLON;
    }  else if ( keycode == 61 ) { // "=" key
        key = KEY_EQUAL;
    } else if ( keycode >= 91 && keycode <= 93 ) { // "[" and "\" and "]" keys
        key = (Keys)(keycode - 45);
    } else if ( keycode == 96 ) { // " ` "
        key = KEY_GRAVE_ACCENT;
    } else if ( keycode >= 256 && keycode <= 269 ) {
        key = (Keys)(keycode - 205);
    }

    // TODO(arthur): finish keycode mapping

    return key;
}

internal str8 GetKeyName(engine_key key)
{
    local_storage const char* keyNames[max_keys] = {
        #define kkey(key, str) str,
        #include "keyboard_keys.def"
        #undef kkey
    };

    str8 result = GetStringFromArray(key, keyNames, max_keys);
    return result;
}

bool32 GetKeyState(engine_key key)
{
    return enginePlatform->currentKeyboard[key];
}

bool32 OnKeyDown(engine_key key)
{
    return enginePlatform->currentKeyboard[key] 
    && !enginePlatform->previousKeyboard[key];
}

bool32 OnKeyUp(engine_key key)
{
    return !enginePlatform->currentKeyboard[key] 
    && enginePlatform->previousKeyboard[key];
}

internal str8 GetMouseButtonName(engine_mouse_button button)
{
    local_storage const char* mouseButtonNames[max_mouse_buttons] = {
        #define mousebutton(button, str) str,
        #include "mouse_buttons.def"
        #undef mousebutton
    };

    str8 result = GetStringFromArray(button, mouseButtonNames, max_mouse_buttons);
    return result;
}

internal bool32 GetMouseButtonState(engine_mouse_button button)
{
    return enginePlatform->currentMouse.buttons[button];
}

internal bool32 OnMouseButtonDown(engine_mouse_button button)
{
    return enginePlatform->currentMouse.buttons[button]
    && !enginePlatform->previousMouse.buttons[button];
}

internal bool32 OnMouseButtonUp(engine_mouse_button button)
{
    return !enginePlatform->currentMouse.buttons[button]
    && enginePlatform->previousMouse.buttons[button];
}

internal v2f GetMousePosition()
{
    return enginePlatform->currentMouse.position;
}

internal v2f GetMouseScroll()
{
    return enginePlatform->currentMouse.scroll;
}
