#!/bin/bash

cd ..
g++ src/platform/macosx_main.cpp -Isrc -framework Cocoa -framework OpenGL -Ltrdp/GLEW/lib -lGLEW -Ltrdp/GLFW/lib -lglfw -o bin/app