#pragma once
#include "core.h"

struct GLFWwindow;

struct window {
    GLFWwindow* nativeWindow;
    unsigned int width;
    unsigned int height;
    const char* title;
};
global_var window application;