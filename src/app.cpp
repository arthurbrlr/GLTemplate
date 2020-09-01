#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "Core/window.h"

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    bool success = CreateWindow();
    if (!success) return -1;

    while (!glfwWindowShouldClose(application.nativeWindow))
    {
        processInput(application.nativeWindow);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(application.nativeWindow);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

