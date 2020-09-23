#pragma once
#include "core.h"

typedef enum engine_controller_button {
    #define controllerbutton(key, str) controller_button_##key,
    #include "controller_buttons.def"
    #undef controllerbutton
    max_controller_buttons
} ControllerButtons;

typedef enum engine_controller_axes {
    #define controlleraxes(axes, str) controller_axes_##axes,
    #include "controller_axes.def"
    #undef controlleraxes
    max_controller_axes
} ControllerAxes;

struct engine_controller {
    bool32 buttons[max_controller_buttons];
    f32 axes[max_controller_axes];
};

internal str8 GetControllerButtonName(engine_controller_button button);
internal bool32 GetControllerButton(int controllerID, engine_controller_button button);
internal bool32 OnControllerButtonDown(int controllerID, engine_controller_button button);
internal bool32 OnControllerButtonUp(int controllerID, engine_controller_button button);

internal str8 GetControllerAxesName(engine_controller_axes axes);
internal f32 GetControllerAxes(int controllerID, engine_controller_axes axes);


    // NOTE(arthur): keyboard default layout is QWERTY
typedef enum engine_key {
    #define kkey(key, str) key,
    #include "keyboard_keys.def"
    #undef kkey
    max_keys
} Keys;
typedef bool32 engine_keyboard[max_keys];

internal engine_key KeyFromGLFWKeycode(int keycode);
internal str8 GetKeyName(engine_key key);
internal bool32 GetKeyState(engine_key key);
internal bool32 OnKeyDown(engine_key key);
internal bool32 OnKeyUp(engine_key key);


typedef enum engine_mouse_button {
    #define mousebutton(button, str) mouse_##button,
    #include "mouse_buttons.def"
    #undef mousebutton
    max_mouse_buttons
} MouseButton;
typedef bool32 engine_mouse_buttons[max_mouse_buttons];

struct engine_mouse
{
    engine_mouse_buttons buttons;
    v2f position;
    v2f delta;
    v2f scroll;
};

internal str8 GetMouseButtonName(engine_mouse_button button);
internal bool32 GetMouseButtonState(engine_mouse_button button);
internal bool32 OnMouseButtonDown(engine_mouse_button button);
internal bool32 OnMouseButtonUp(engine_mouse_button button);
internal v2f GetMousePosition();
internal v2f GetMouseScroll();