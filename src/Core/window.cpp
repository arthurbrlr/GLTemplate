#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "events.h"
#include "platform.h"

#include "log.cpp"

internal void PlatformWindowCloseCallback(GLFWwindow* window)
{
    PlatformRegisterEvent(platform_event{EventTypes::application_close});
}

internal void PlatformWindowResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

internal void PlatformDragDropCallback(GLFWwindow* window, int count, const char** paths)
{
    for (int i = 0; i < count; i++) {
        //handle_dropped_file(paths[i]);
        LogMsg("File dropped: %s\n", paths[i]);
    }
}

internal void PlatformKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    platform_event pEvent = {};
    pEvent.glfwKeyCode = key;
    switch (action)
    {
        case GLFW_PRESS:
        {
            pEvent.type = EventTypes::key_pressed;
        } break;

        case GLFW_RELEASE:
        {
            pEvent.type = EventTypes::key_released;
        } break;
    
        default:
            break;
    }
    PlatformRegisterEvent(pEvent);
}

internal void PlatformJoystickCallback(int jid, int event)
{
    platform_event pEvent = {};
    pEvent.controller.ID = (i8)jid;
    if (event == GLFW_CONNECTED) {
        pEvent.type = EventTypes::controller_connect;
    } else if (event == GLFW_DISCONNECTED) {
        pEvent.type = EventTypes::controller_disconnect;
    }
    PlatformRegisterEvent(pEvent);
}

internal void PlatformCursorEnterCallback(GLFWwindow* window, int enter)
{
    if ( enter) {
        PlatformRegisterEvent(platform_event{mouse_enter});
    } else {
        PlatformRegisterEvent(platform_event{mouse_leave});
    }
}

internal void PlatformMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    platform_event pEvent;
    pEvent.mouse.button = (engine_mouse_button)button;
    if ( action == GLFW_PRESS ) {
        pEvent.type = EventTypes::mouse_pressed;
    } else {
        pEvent.type = EventTypes::mouse_released;
    }
    PlatformRegisterEvent(pEvent);
}

internal void PlatformMouseScrolledCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    platform_event pEvent;
    pEvent.type = EventTypes::mouse_scrolled;
    pEvent.mouse.scroll = v2f{ (f32)xoffset, (f32)yoffset };
    PlatformRegisterEvent(pEvent);
}

internal void PlatformMouseMovedCallback(GLFWwindow* window, double xpos, double ypos)
{
    platform_event pEvent;
    pEvent.type = EventTypes::mouse_moved;
    pEvent.mouse.position = v2f{ (f32)xpos, (f32)ypos };
    PlatformRegisterEvent(pEvent);

    // TODO(arthur): compute delta between current and previous mouse position
}

internal void PlatformProcessInputs(GLFWwindow *window)
{
    GLFWgamepadstate state;
    platform_event pEvent = {};
    for (int controllerID = 0; controllerID < 4; controllerID++) {
        if (glfwGetGamepadState(controllerID, &state)) {

            pEvent.controller.ID = (i8)controllerID;
            for (int buttonID = 0; buttonID < max_controller_buttons; buttonID++) {
                pEvent.controller.button = (ControllerButtons)buttonID;
                if (state.buttons[buttonID]) {
                    pEvent.type = EventTypes::controller_button_pressed;
                } else {
                    pEvent.type = EventTypes::controller_button_released;
                }
                PlatformRegisterEvent(pEvent);
            }

            for (int axesID = 0; axesID < max_controller_axes; axesID++) {
                pEvent.type = EventTypes::controller_axes_moved;
                pEvent.controller.axes = (ControllerAxes)axesID;
                pEvent.controller.axesAmount = state.axes[axesID];
                PlatformRegisterEvent(pEvent);             
            }

        }
    }
}