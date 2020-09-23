#pragma once
#include "core.h"
#include "events.h"

typedef enum platform_event_type {
    application_start,
    application_close,
    application_minimise,
    
    file_open,
    file_read,
    file_write,
    file_close,

    key_pressed,
    key_released,

    mouse_enter,
    mouse_leave,
    mouse_moved,
    mouse_pressed,
    mouse_released,
    mouse_scrolled,

    controller_connect,
    controller_disconnect,
    controller_button_pressed,
    controller_button_released,
    controller_axes_moved,

    max_event_types
} EventTypes;

struct platform_event {
    platform_event_type type;
    union {
        str8 file;
        int glfwKeyCode;
        struct {
            i8 ID;
            engine_controller_button button;
            engine_controller_axes axes;
            f32 axesAmount;
        } controller;
        struct {
            v2f position;
            v2f delta;
            v2f scroll;
            engine_mouse_button button;
        } mouse;
    };
};

internal void PlatformRegisterEvent(platform_event event);
internal void PlatformResetEventQueue();


struct platform {
    bool32 run;

    i16 eventCount;
    i16 maxEventsPerFrame;
    platform_event events[1024];

    engine_keyboard previousKeyboard;
    engine_keyboard currentKeyboard;

    i16 connectedControllers;
    engine_controller previousControllers[4];
    engine_controller currentControllers[4];

    bool32 mouseInWindow;
    engine_mouse currentMouse;
    engine_mouse previousMouse;
};