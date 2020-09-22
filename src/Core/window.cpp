#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "window.h"
#include "log.cpp"

internal void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

internal void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        LogWarn("Joystick %d connected\n", jid);
    }
    else if (event == GLFW_DISCONNECTED)
    {
        LogWarn("Joystick %d disconnected\n", jid);
    }
}

internal void PlatformProcessInputs(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    GLFWgamepadstate state;
    if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
    {
        if (state.buttons[GLFW_GAMEPAD_BUTTON_A])
        {
            LogMsg("Button A is pressed on controller 0\n");
        }
    }
}