#!/bin/bash

cd ..

g++ src/app.cpp -framework Cocoa -framework OpenGL -Ltrdp/GLEW/lib -lGLEW -Ltrdp/GLFW/lib -lglfw -o bin/app