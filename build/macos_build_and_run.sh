#!/bin/bash

cd ..

includes="-Isrc -Itrdp/GLEW/include -Itrdp/GLFW/include"
libs="-framework Cocoa -framework OpenGL -Ltrdp/GLEW/lib -lGLEW -Ltrdp/GLFW/lib -lglfw"
opts="-g -std=c++17 -O2"

echo "dylib"
g++ -dynamiclib -fPIC $opt $includes src/engine.cpp -o bin/candle.dylib
echo "app"
g++ $opts $includes $libs src/platform/macosx_main.cpp -o bin/app