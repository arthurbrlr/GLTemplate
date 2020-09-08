#!/bin/bash

cd ..

if [ "$1" == "release" ]; then
    ./build/macos/premake5 gmake2
    rm bin/bin/Release/GLTemplate
    make config=release
    ./bin/bin/Release/GLTemplate
elif [ "$1" == "clean" ]; then
    ./build/macos/premake5 clean
    make clean
else
    ./build/macos/premake5 gmake2
    rm bin/bin/Debug/GLTemplate
    make config=debug
    ./bin/bin/Debug/GLTemplate
fi