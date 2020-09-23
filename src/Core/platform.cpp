#include "platform.h"
#include "log.h"

#include "events.cpp"

global_var platform globalOS;

internal void PlatformRegisterEvent(platform_event event)
{
    if ( globalOS.eventCount < globalOS.maxEventsPerFrame ) {
        globalOS.events[globalOS.eventCount++] = event;

        switch (event.type)
        {
            case EventTypes::application_start:
            {
                globalOS.run = true;
            } break;

            case EventTypes::application_close:
            {
                globalOS.run = false;
            } break;

            case EventTypes::key_pressed:
            {
                engine_key eKey = KeyFromGLFWKeycode(event.glfwKeyCode);
                globalOS.currentKeyboard[eKey] = true;
                LogMsg("Key pressed: %s (%d)\n", GetKeyName(eKey).string, eKey);
            } break;

            case EventTypes::key_released:
            {
                engine_key eKey = KeyFromGLFWKeycode(event.glfwKeyCode);
                globalOS.currentKeyboard[eKey] = false;
            } break;

            case EventTypes::controller_connect:
            {
                LogWarn("Joystick %d connected\n", event.controller.ID);
                globalOS.connectedControllers++;
            } break;

            case EventTypes::controller_disconnect:
            {
                LogWarn("Joystick %d disconnected\n", event.controller.ID);
                globalOS.connectedControllers--;
            } break;

            case EventTypes::controller_button_pressed:
            {
                engine_controller_button eButton = event.controller.button;
                globalOS.currentControllers[event.controller.ID].buttons[eButton] = true;
            } break;

            case EventTypes::controller_button_released:
            {
                engine_controller_button eButton = event.controller.button;
                globalOS.currentControllers[event.controller.ID].buttons[eButton] = false;
            } break;
        
            case EventTypes::controller_axes_moved:
            {
                engine_controller_axes eAxes = event.controller.axes;
                globalOS.currentControllers[event.controller.ID].axes[eAxes] = event.controller.axesAmount;
            } break;

            case EventTypes::mouse_enter:
            {
                globalOS.mouseInWindow = true;
            } break;

            case EventTypes::mouse_leave:
            {
                globalOS.mouseInWindow = false;
            } break;

            case EventTypes::mouse_pressed:
            {
                globalOS.currentMouse.buttons[event.mouse.button] = true;
            } break;

            case EventTypes::mouse_released:
            {
                globalOS.currentMouse.buttons[event.mouse.button] = false;
            } break;

            case EventTypes::mouse_scrolled:
            {
                globalOS.currentMouse.scroll = event.mouse.scroll;
            } break;

            case EventTypes::mouse_moved:
            {
                globalOS.currentMouse.position = event.mouse.position;
            } break;

            default:
            {

            } break;
        }
    }
}

internal void PlatformResetEventQueue()
{
    globalOS.eventCount = 0;
    ARRAY_COPY(globalOS.currentKeyboard, globalOS.previousKeyboard);
    ARRAY_COPY(globalOS.currentControllers, globalOS.previousControllers);
    globalOS.previousMouse = globalOS.currentMouse;
}