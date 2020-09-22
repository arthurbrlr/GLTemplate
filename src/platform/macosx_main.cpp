#ifdef __APPLE__

#include "core/window.cpp"
#include "engine.h"

#include <dlfcn.h> // dylib 
#include <sys/stat.h> // filestat
#include <fcntl.h> // open
#include <unistd.h> // close
#include <stdlib.h> // malloc, free

struct osx_engine_code {
    bool32 isValid;
	size_t dylibLastUpadteTime;
    void* dylibHandle;

    EngineUpdateFunc* Update;
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

internal void DEBUGPlatformFreeFileMemory(void* memory)
{
	if (memory) {
		free(memory);
	}
}

internal debug_platform_file DEBUGPlatformReadEntireFile(const char* filepath)
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


internal bool32 DEBUGPlatformWriteEntireFile(debug_platform_file* file, const char* filepath)
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



internal osx_engine_code OSXLoadEngineCode(const char* dylibPath, const char* dylibTempPath)
{
    osx_engine_code engineCode = {};
    engineCode.dylibLastUpadteTime = OSXGetLastUpdateTime(dylibPath);
    engineCode.isValid = false;

    debug_platform_file dylib = DEBUGPlatformReadEntireFile(dylibPath);
    bool32 copyDone = false;
    if ( dylib.content ) {
        copyDone = DEBUGPlatformWriteEntireFile(&dylib, dylibTempPath);
        DEBUGPlatformFreeFileMemory(dylib.content);
    }

    if ( copyDone ) {
        engineCode.dylibHandle = dlopen(dylibTempPath, RTLD_LAZY | RTLD_GLOBAL);
        if ( engineCode.dylibHandle )
        {
            engineCode.Update = (EngineUpdateFunc*)dlsym(engineCode.dylibHandle, "EngineUpdateAndRender");
            if ( engineCode.Update ) {
                engineCode.isValid = true;
            }
        } else {
            LogErr("Couldn't open engine dylib\n");
        }
        LogInfo("LastUpdateTime for %s : %d\n", dylibTempPath, engineCode.dylibLastUpadteTime);
    }

	if ( !engineCode.isValid )
	{
		engineCode.Update = EngineUpdate__Undefined;
	}

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
    glfwSetFramebufferSizeCallback(application.nativeWindow, framebuffer_size_callback);
    glfwSetJoystickCallback(joystick_callback);

    GLenum err = glewInit();
    if ( err != GLEW_OK ) {
        LogErr("Error: %s\n", glewGetErrorString(err));
        return -1;
    }

    LogInfo("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    osx_engine_code engine = OSXLoadEngineCode("candle.dylib", "candle_temp.dylib");

    while (!glfwWindowShouldClose(application.nativeWindow))
    {
        size_t engineCodeUpdate = OSXGetLastUpdateTime("candle.dylib");
        if ( engineCodeUpdate > engine.dylibLastUpadteTime ) {
            OSXUnloadEngineCode(&engine);
            engine = OSXLoadEngineCode("candle.dylib", "candle_temp.dylib");
        }


        PlatformProcessInputs(application.nativeWindow);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        engine.Update();

        glfwSwapBuffers(application.nativeWindow);
        glfwPollEvents();  
    }

    OSXUnloadEngineCode(&engine);

    glfwTerminate();
    return 0;
}


#endif