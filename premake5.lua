#!lua

solution "GLTemplate"
    configurations { "Debug", "Release" }

    project "GLTemplate"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"

        objdir "bin-int/%{cfg.platform}/%{cfg.buildcfg}"
        architecture "x64"

        files {
            "src/**.h",
            "src/**.cpp"
        }

        includedirs {
            "trdp/GLEW/include",
            "trdp/GLFW/include"
		}

        -- libraries
        libdirs {
            "trdp/GLEW/lib",
            "trdp/GLFW/lib" 
        }


        configuration "windows"
            links {
                "glew32s",
                "glfw3",
                "opengl32.lib"
            }

            defines {
                "GLEW_STATIC"
			}

            ignoredefaultlibraries { 
                "MSVCRT" 
            }

        configuration "linux"
            links {
                "GLEW",
                "glfw"
            }
        
        configuration "macosx"
            links {
                "GLEW",
                "glfw",
                "Cocoa.framework",
                "OpenGL.framework",
                "IOKit.framework",
                "CoreVideo.framework"
            }
 
        -- configs
        configuration "Debug"
        defines { "_DEBUG", "DEBUG", "GLT_DEBUG" }
        symbols "On"

        configuration "Release"
        defines { "_NDEBUG", "NDEBUG", "GLT_RELEASE" }
        optimize "On"
