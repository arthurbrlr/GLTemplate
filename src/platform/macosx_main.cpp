#ifdef __APPLE__

#include "core/platform.cpp"
#include "core/filesystem.h"
#include "engine.h"

#include "core/window.cpp"

#include <dlfcn.h> // dylib 
#include <sys/stat.h> // filestat
#include <fcntl.h> // open
#include <unistd.h> // close
#include <stdlib.h> // malloc, free
#include <sys/mman.h> // mmap
#include <libproc.h> // pid

struct osx_engine_code {
    bool32 isValid;
	size_t dylibLastUpadteTime;
    void* dylibHandle;

    EngineUpdateFunc* Update;
    EngineSetPlatformFunc* SetPlatform;
};

struct platform_state {
    char appFilepath[PROC_PIDPATHINFO_MAXSIZE];
    char* lastSlashPlusOne;
};

struct debug_platform_file {
    u32 size;
    void* content;
};

internal size_t OSXGetLastUpdateTime(const char* filepath)
{
	time_t lastUpdateTime = 0;

	struct stat filestat;
	if ( stat(filepath, &filestat) == 0 )
	{
		lastUpdateTime = filestat.st_mtimespec.tv_sec;
	}

	return lastUpdateTime;
}

internal void OSXGetExecutableFilename(platform_state* platformPtr)
{
    pid_t PID = getpid();
	int pidPathRes = proc_pidpath(PID, platformPtr->appFilepath, sizeof(platformPtr->appFilepath));

	if ( pidPathRes <= 0 ) {
		LogErr("OSXGetExecutableFilename: Error while getting process path for PID %d\n", PID);
	}

    platformPtr->lastSlashPlusOne = platformPtr->appFilepath;
    for(char *scanIndex = platformPtr->lastSlashPlusOne; *scanIndex; ++scanIndex) {
        if(*scanIndex == '/') {
            platformPtr->lastSlashPlusOne = scanIndex + 1;
        }
    }
}

DEBUG_PLATFORM_IO_FREEFILEMEMORY(DEBUGPlatformFreeFileMemory)
{
	if (fileMemory) {
		free(fileMemory);
	}
}

DEBUG_PLATFORM_IO_READENTIREFILE(DEBUGPlatformReadEntireFile)
{
	debug_platform_file result = {};

	int fd = open(filepath, O_RDONLY);
	if (fd != -1) {
		struct stat fileStat;

		if (fstat(fd, &fileStat) == 0) {
			int allocSuccess = -1;
			result.size = fileStat.st_size;
			result.content = (char*)malloc(result.size);

			if (result.content) {
				allocSuccess = 0;
			}

			if (allocSuccess == 0) {
				ssize_t bytesRead;
				bytesRead = read(fd, result.content, result.size);

				if (bytesRead != result.size) {
					DEBUGPlatformFreeFileMemory(result.content);
					result.content = 0;
				}
			} else {
				LogErr("DEBUGPlatformReadEntireFile: Couldn't allocate %d bytes while reading %s\n", result.size, filepath);
			}
		} else {
			LogErr("DEBUGPlatformReadEntireFile: Stat reading for %s\n", filepath);
		}

		close(fd);
	} else {
        LogErr("DEBUGPlatformReadEntireFile: Couldn't open %s\n", filepath);
	}

	return result;
}


DEBUG_PLATFORM_IO_WRITEENTIREFILE(DEBUGPlatformWriteEntireFile)
{
	bool32 result = false;

	int fd = open(filepath, O_WRONLY | O_CREAT, 0644);
	if (fd != -1) {
		ssize_t bytesWritten = write(fd, file->content, file->size);
		result = ( bytesWritten == file->size );

		if (!result) {
			LogErr("DEBUGPlatformWriteEntireFile: File was not correctly written (%d bytes written, %d expected)\n", bytesWritten, file->size);
		}

		close(fd);
	} else {
        LogErr("DEBUGPlatformWriteEntireFile: Couldn't open %s\n", filepath);
	}

	return result;
}



internal osx_engine_code OSXLoadEngineCode(const char* dylibName, const char* dylibTempName)
{
    platform_state plt = {};
    OSXGetExecutableFilename(&plt);

    char dylibFullPath[PROC_PIDPATHINFO_MAXSIZE];
    ConcatString(plt.lastSlashPlusOne - plt.appFilepath, plt.appFilepath,
                 GetCStringLength(dylibName), dylibName,
                 GetCStringLength(dylibFullPath), dylibFullPath);

    char dylibTempFullPath[PROC_PIDPATHINFO_MAXSIZE];
    ConcatString(plt.lastSlashPlusOne - plt.appFilepath, plt.appFilepath,
                 GetCStringLength(dylibTempName), dylibTempName,
                 GetCStringLength(dylibTempFullPath), dylibTempFullPath);

    osx_engine_code engineCode = {};
    engineCode.dylibLastUpadteTime = OSXGetLastUpdateTime(dylibFullPath);
    engineCode.isValid = false;

    debug_platform_file dylib = DEBUGPlatformReadEntireFile(dylibFullPath);
    bool32 copyDone = false;
    if ( dylib.content ) {
        copyDone = DEBUGPlatformWriteEntireFile(&dylib, dylibTempFullPath);
        DEBUGPlatformFreeFileMemory(dylib.content);
    }

    if ( copyDone ) {
        engineCode.dylibHandle = dlopen(dylibTempFullPath, RTLD_LAZY | RTLD_GLOBAL);
        if ( engineCode.dylibHandle ) {
            engineCode.Update = (EngineUpdateFunc*)dlsym(engineCode.dylibHandle, "EngineUpdateAndRender");
            engineCode.SetPlatform = (EngineSetPlatformFunc*)dlsym(engineCode.dylibHandle, "EngineSetPlatform");

            engineCode.isValid = (bool32)( engineCode.Update || engineCode.SetPlatform );
        } else {
            LogErr("Couldn't open engine dylib\n");
        }
    }

	if ( !engineCode.isValid ) {
		engineCode.Update = EngineUpdate__Undefined;
        engineCode.SetPlatform = EngineSetPlatform__Undefined;
	}

    engineCode.SetPlatform(&globalOS);

	return engineCode;
}

internal void OSXUnloadEngineCode(osx_engine_code* engine)
{
    if ( engine->dylibHandle ) {
        dlclose(engine->dylibHandle);
        engine->dylibHandle = 0;
    }

    engine->isValid = false;
    engine->Update = EngineUpdate__Undefined;
}

int main()
{
    application.width = 800;
    application.height = 600;
    application.title = "GLTemplate_MACOS";

    if ( glfwInit() == GLFW_FALSE ) {
        LogErr("Error: Failed at glfw init\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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

    osx_engine_code engine = OSXLoadEngineCode("candle.dylib", "candle_temp.dylib");
    if ( !engine.isValid ) {
        LogErr("Engine dylib was not initialised correctly\n");
    }
    globalOS.maxEventsPerFrame = 1024;

    engine_memory engineMemory = {};
    engineMemory.permanentContainerSize = (u64)MegaBytes(64);
    engineMemory.transientContainerSize = (u64)GigaBytes(1);
    u64 totalMemorySize = engineMemory.permanentContainerSize + engineMemory.transientContainerSize;
    engineMemory.permanentContainer = mmap(0, totalMemorySize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    engineMemory.transientContainer = ( (u8*)(engineMemory.permanentContainer) + engineMemory.permanentContainerSize );

    if  ( !engineMemory.permanentContainer || !engineMemory.transientContainer ) {
        LogErr("Engine memory allocation failed\n");
    }
    PlatformRegisterEvent(platform_event{EventTypes::application_start});

    platform_state plt = {};
    OSXGetExecutableFilename(&plt);
    char dylibFullPath[PROC_PIDPATHINFO_MAXSIZE];
    ConcatString(plt.lastSlashPlusOne - plt.appFilepath, plt.appFilepath,
                 GetCStringLength("engine.dll"), "engine.dll",
                 GetCStringLength(dylibFullPath), dylibFullPath);

    while ( globalOS.run ) {

        size_t engineCodeUpdate = OSXGetLastUpdateTime(dylibFullPath);
        if ( engineCodeUpdate > engine.dylibLastUpadteTime ) {
            OSXUnloadEngineCode(&engine);
            sleep(1);
            engine = OSXLoadEngineCode("candle.dylib", "candle_temp.dylib");
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

    OSXUnloadEngineCode(&engine);

    glfwTerminate();
    return 0;
}


#endif