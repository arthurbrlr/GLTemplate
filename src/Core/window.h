#pragma once
#include "core.h"

struct GLFWwindow;

struct window {
    GLFWwindow* nativeWindow;
    unsigned int width;
    unsigned int height;
    char* title;
};
global_var window application;

internal bool window_create();
internal bool window_sould_close();
internal void window_update();
internal void window_free();
