#pragma once
#include "core.h"

struct GLFWwindow;

struct platform_window {
    GLFWwindow* nativeWindow;
    unsigned int width;
    unsigned int height;
    const char* title;
};
global_var platform_window application;

internal void PlatformWindowCloseCallback(GLFWwindow* window);
internal void PlatformWindowResizeCallback(GLFWwindow* window, int width, int height);
internal void PlatformDragDropCallback(GLFWwindow* window, int count, const char** paths);

internal void PlatformKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
internal void PlatformJoystickCallback(int jid, int event);
internal void PlatformCursorEnterCallback(GLFWwindow* window, int enter);
internal void PlatformMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
internal void PlatformMouseScrolledCallback(GLFWwindow* window, double xoffset, double yoffset);
internal void PlatformMouseMovedCallback(GLFWwindow* window, double xpos, double ypos);

internal void PlatformProcessInputs(GLFWwindow *window);