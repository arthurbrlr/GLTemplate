#ifdef _WIN64

#include "core/window.cpp"
#include "app.cpp"
#include <GLFW/glfw3.h>

int main()
{
    application.height = 600;
    application.width = 800;
    application.title = "GLTemplate_MACOS";

    if ( glfwInit() == GLFW_FALSE ) {
        LogErr("Error: Failed at glfw init\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    application.nativeWindow = glfwCreateWindow(application.width, application.height, application.title, NULL, NULL);
    if (application.nativeWindow == NULL) {
        LogErr("Error: Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(application.nativeWindow);
    glfwSetFramebufferSizeCallback(application.nativeWindow, framebuffer_size_callback);
    glfwSetJoystickCallback(joystick_callback);

    GLenum err = glewInit();
    if ( err != GLEW_OK ) {
        LogErr("Error: %s\n", glewGetErrorString(err));
        return -1;
    }

    LogInfo("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    while (!glfwWindowShouldClose(application.nativeWindow))
    {
        PlatformProcessInputs(application.nativeWindow);

        ApplicationUpdate();

        glfwSwapBuffers(application.nativeWindow);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

#endif