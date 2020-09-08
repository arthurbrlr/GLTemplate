#!lua

solution "GLTemplate"
    configurations { "Debug", "Release" }

    project "GLTemplate"
        kind "ConsoleApp"
        language "C++"
        location "bin"
        objdir "bin-int/%{cfg.platform}/%{cfg.buildcfg}"
        buildoptions { "-std=c++17" }
        architecture "x64"

        files {
            "src/**.h",
            "src/**.cpp"
        }

        -- libraries
        libdirs {
            "trdp/GLEW/lib",
            "trdp/GLFW/lib" 
        }

        configuration "windows"
            links {
                "GLEW",
                "glfw"
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
