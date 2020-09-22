#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "window.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}


window application = window();
bool window_create()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    application.nativeWindow = glfwCreateWindow(application.width, application.height, "OpenGL", NULL, NULL);
    if (application.nativeWindow == NULL)
    {
        fprintf(stdout, "Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(application.nativeWindow);
    glfwSetFramebufferSizeCallback(application.nativeWindow, framebuffer_size_callback);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return false;
    }

    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    return true;
}


bool window_sould_close()
{
    return glfwWindowShouldClose(application.nativeWindow);
}


void window_update()
{
    processInput(application.nativeWindow);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(application.nativeWindow);
    glfwPollEvents();
}


void window_free()
{
    glfwTerminate();
}