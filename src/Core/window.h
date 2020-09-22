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