#ifdef _WIN64

#include "core/platform.cpp"
#include "core/filesystem.h"
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

internal void Win32GetExecutableFilename(platform_state* platformPtr)
{
    DWORD sizeofFilename = GetModuleFileNameA(0, platformPtr->appFilepath, sizeof(platformPtr->appFilepath));

    platformPtr->lastSlashPlusOne = platformPtr->appFilepath;
    for(char *scanIndex = platformPtr->lastSlashPlusOne; *scanIndex; ++scanIndex) {
        if(*scanIndex == '\\') {
            platformPtr->lastSlashPlusOne = scanIndex + 1;
        }
    }
}

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

DEBUG_PLATFORM_IO_FREEFILEMEMORY(DEBUGPlatformFreeFileMemory)
{
    if ( fileMemory ) {
        VirtualFree(fileMemory, 0, MEM_RELEASE);
    }  
}

DEBUG_PLATFORM_IO_READENTIREFILE(DEBUGPlatformReadEntireFile)
{
    debug_file result = {};
    HANDLE fileHandle = CreateFileA(filepath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if ( fileHandle != INVALID_HANDLE_VALUE ) {
        LARGE_INTEGER filesize;
        if ( GetFileSizeEx(fileHandle, &filesize) ) {
            u32 filesize32 = SafeTruncateU64toU32(filesize.QuadPart);
            result.content = VirtualAlloc(0, filesize32, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if ( result.content ) {
                DWORD bytesRead;
                if ( ReadFile(fileHandle, result.content, (DWORD)filesize.QuadPart, &bytesRead, 0) && (bytesRead == filesize32) ) {
                    result.size = bytesRead;
                } else {
                    DEBUGPlatformFreeFileMemory(result.content);
                    result.content = 0;
                    result.size = 0;
                    LogErr("DEBUGPlatformReadEntireFile: Error while reading file %d: bytesRead != filesize\n", filepath);
                }
            } else {
                LogErr("DEBUGPlatformReadEntireFile: Error while allocating memory\n");
            }
        } else {
            LogErr("DEBUGPlatformReadEntireFile: Couldn't get filesize for %s\n", filepath);
        }
        CloseHandle(fileHandle);
    } else {
        LogErr("DEBUGPlatformReadEntireFile: couldn't open file %s\n", filepath);
    }
    return result;
}

DEBUG_PLATFORM_IO_WRITEENTIREFILE(DEBUGPlatformWriteEntireFile)
{
    bool32 result = false;
    HANDLE fileHandle = CreateFileA(filepath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if ( fileHandle != INVALID_HANDLE_VALUE ) {
        DWORD bytesWritten;
        if ( WriteFile(fileHandle, file->content, file->size, &bytesWritten, 0) ) {
            result = ( bytesWritten == file->size );
        } else {
            LogErr("DEBUGPlatformWriteEntireFile: Error while writing to file %s\n", filepath);
        }
        CloseHandle(fileHandle);
    } else {
        LogErr("DEBUGPlatformWriteEntireFile: Error while creating file %s\n", filepath);
    }
    return result;
}

internal win32_engine_code Win32LoadEngineCode(const char* dllName, const char* dllTempName)
{
    platform_state plt = {};
    Win32GetExecutableFilename(&plt);

    char dllFullPath[MAX_PATH];
    ConcatString(plt.lastSlashPlusOne - plt.appFilepath, plt.appFilepath,
                 GetCStringLength(dllName), dllName,
                 GetCStringLength(dllFullPath), dllFullPath);
    
    char dllTempFullPath[MAX_PATH];
    ConcatString(plt.lastSlashPlusOne - plt.appFilepath, plt.appFilepath,
                 GetCStringLength(dllTempName), dllTempName,
                 GetCStringLength(dllTempFullPath), dllTempFullPath);

    win32_engine_code engineCode = {};
    engineCode.dllLastUpdateTime = Win32GetLastUpdateTime(dllFullPath);

    bool32 copyDone = CopyFileA(dllFullPath, dllTempFullPath, FALSE);
    if ( copyDone ) {
        engineCode.dllHandle = LoadLibraryA(dllTempFullPath);
        if ( engineCode.dllHandle ) {
            engineCode.Update = (EngineUpdateFunc*)GetProcAddress(engineCode.dllHandle, "EngineUpdateAndRender");
            engineCode.SetPlatform = (EngineSetPlatformFunc*)GetProcAddress(engineCode.dllHandle, "EngineSetPlatform");

            engineCode.isValid = (bool32)( engineCode.Update || engineCode.SetPlatform );
        } else {
            LogErr("Couldn't open engine dll\n");
        }
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

    engineMemory.freefile = DEBUGPlatformFreeFileMemory;
    engineMemory.readfile = DEBUGPlatformReadEntireFile;
    engineMemory.writefile = DEBUGPlatformWriteEntireFile;

    if  ( !engineMemory.permanentContainer || !engineMemory.transientContainer ) {
        LogErr("Engine memory allocation failed\n");
    }
    PlatformRegisterEvent(platform_event{EventTypes::application_start});

    platform_state plt = {};
    Win32GetExecutableFilename(&plt);
    char dllFullPath[MAX_PATH];
    ConcatString(plt.lastSlashPlusOne - plt.appFilepath, plt.appFilepath,
                 GetCStringLength("engine.dll"), "engine.dll",
                 GetCStringLength(dllFullPath), dllFullPath);

    while (!glfwWindowShouldClose(application.nativeWindow))
    {
        FILETIME engineCodeUpdate = Win32GetLastUpdateTime(dllFullPath);
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

    Win32UnloadEngineCode(&engine);

    glfwTerminate();
    return 0;
}

#endif