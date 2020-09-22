#!/bin/bash

cd ..

includes="-Isrc -Itrdp/GLEW/include -Itrdp/GLFW/include"
libs="-framework Cocoa -framework OpenGL -Ltrdp/GLEW/lib -lGLEW -Ltrdp/GLFW/lib -lglfw"
opts="-g"

g++ -dynamiclib -fPIC $opt $includes src/engine.cpp -o bin/candle.dylib
g++ $opts $includes $libs src/platform/macosx_main.cpp -o bin/app