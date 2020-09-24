#ifdef _WIN64

#include "core/platform.cpp"
#include "engine.h"

#include "core/window.cpp"

#include <windows.h>

struct win32_engine_code {
    HMODULE dllHandle;
    FILETIME dllLastUpdateTime;
    bool32 isValid;

    EngineUpdateFunc* Update;
    EngineSetPlatformFunc* SetPlatform;
};

struct platform_state {
    char appFilepath[MAX_PATH];
    char* lastSlashPlusOne;
};

internal FILETIME Win32GetLastUpdateTime(const char* filename)
{
    FILETIME lastUpdate = {};
    WIN32_FIND_DATAA findData;
    HANDLE findHandle = FindFirstFileA(filename, &findData);

    if ( findHandle != INVALID_HANDLE_VALUE ) {
        lastUpdate = findData.ftLastWriteTime;
        FindClose(findHandle);
    }

    return lastUpdate;
}

internal win32_engine_code Win32LoadEngineCode(const char* dllName, const char* dllTempName)
{
    const char* dllFilepath = "engine.dll";
    const char* tempDllFilepath = "engine_temp.dll";
    
    win32_engine_code engineCode = {};

    CopyFileA(dllFilepath, tempDllFilepath, FALSE);
    HMODULE lighterLib = LoadLibraryA(tempDllFilepath);
    
    if ( lighterLib ) {
        engineCode.dllHandle = lighterLib;
        engineCode.dllLastUpdateTime = Win32GetLastUpdateTime(dllFilepath);

        engineCode.Update = (EngineUpdateFunc*)GetProcAddress(lighterLib, "EngineUpdateAndRender");
        engineCode.SetPlatform = (EngineSetPlatformFunc*)GetProcAddress(lighterLib, "EngineSetPlatform");

        engineCode.isValid = (bool)( engineCode.Update || engineCode.SetPlatform );
    }

    if ( !engineCode.isValid ) {
        engineCode.Update = EngineUpdate__Undefined;
        engineCode.SetPlatform = EngineSetPlatform__Undefined;
    }

    engineCode.SetPlatform(&globalOS);

    return engineCode;
}

internal void Win32UnloadEngineCode(win32_engine_code* engineCode)
{
    if ( engineCode->isValid ) {
        FreeLibrary(engineCode->dllHandle);
    }

    engineCode->isValid = false;
    engineCode->Update = EngineUpdate__Undefined;
    engineCode->SetPlatform = EngineSetPlatform__Undefined;
}

int main()
{
    application.width = 800;
    application.height = 600;
    application.title = "GLTemplate_WINDOWS";

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
    
    glfwSetWindowCloseCallback(application.nativeWindow, PlatformWindowCloseCallback);
    glfwSetFramebufferSizeCallback(application.nativeWindow, PlatformWindowResizeCallback);
    glfwSetDropCallback(application.nativeWindow, PlatformDragDropCallback);

    glfwSetKeyCallback(application.nativeWindow, PlatformKeyCallback);
    glfwSetJoystickCallback(PlatformJoystickCallback);
    glfwSetCursorEnterCallback(application.nativeWindow, PlatformCursorEnterCallback);
    glfwSetMouseButtonCallback(application.nativeWindow, PlatformMouseButtonCallback);
    glfwSetScrollCallback(application.nativeWindow, PlatformMouseScrolledCallback);
    glfwSetCursorPosCallback(application.nativeWindow, PlatformMouseMovedCallback);

    GLenum err = glewInit();
    if ( err != GLEW_OK ) {
        LogErr("Error: %s\n", glewGetErrorString(err));
        return -1;
    }

    LogInfo("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    win32_engine_code engine = Win32LoadEngineCode("engine.dll", "engine_temp.dll");
    if ( !engine.isValid ) {
        LogErr("Engine dll was not initialised correctly\n");
    }
    globalOS.maxEventsPerFrame = 1024;

    engine_memory engineMemory = {};
    engineMemory.permanentContainerSize = (u64)MegaBytes(64);
    engineMemory.transientContainerSize = (u64)GigaBytes(1);
    u64 totalMemorySize = engineMemory.permanentContainerSize + engineMemory.transientContainerSize;
    engineMemory.permanentContainer = VirtualAlloc(0, (SIZE_T)totalMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    engineMemory.transientContainer = ( (u8*)(engineMemory.permanentContainer) + engineMemory.permanentContainerSize );

    if  ( !engineMemory.permanentContainer || !engineMemory.transientContainer ) {
        LogErr("Engine memory allocation failed\n");
    }
    PlatformRegisterEvent(platform_event{EventTypes::application_start});

    while (!glfwWindowShouldClose(application.nativeWindow))
    {
        FILETIME engineCodeUpdate = Win32GetLastUpdateTime("engine.dll");
        if ( CompareFileTime(&engineCodeUpdate, &engine.dllLastUpdateTime) ) {
            Win32UnloadEngineCode(&engine);
            Sleep(100);
            engine = Win32LoadEngineCode("engine.dll", "engine_temp.dll");
            LogInfo("Hot reloading !\n");
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        engine.Update(&engineMemory);

        glfwSwapBuffers(application.nativeWindow);

        // NOTE(arthur): use glfwPollEvents if the app needs real time update
        //               use glfwWaitEvents if the application doesn't need to be updated at
        //               a constant time interval (less CPU intensive)
        PlatformResetEventQueue();
        glfwPollEvents();
        PlatformProcessInputs(application.nativeWindow);
    }

    glfwTerminate();
    return 0;
}

#endif