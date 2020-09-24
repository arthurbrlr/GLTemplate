@echo off
pushd %~dp0\..\bin

set commonCompilerFlags=-MT -nologo -Oi -WX -W4 -wd4505 -wd4100 -wd4189 -wd4273 -wd4005 -FAsc -Zi -I "../src/" -I "../trdp/GLEW/include" -I "../trdp/GLFW/include"
set commonLinkerFlags=-incremental:no -opt:ref -LIBPATH:"../trdp/GLEW/lib" -LIBPATH:"../trdp/GLFW/lib" glew32s.lib glfw3.lib gdi32.lib opengl32.lib user32.lib kernel32.lib msvcrt.lib shell32.lib /NODEFAULTLIB:LIBCMT
:: compile program // -GR- -EHa- 
    :: x86
:: cl %commonCompilerFlags% ..\src\win32_main.cpp /link -subsystem:windows,5.1 %commonLinkerFlags%
    :: x64

del *.pdb > NUL 2> NUL
cl %commonCompilerFlags% /LD ..\src\engine.cpp /link -EXPORT:EngineUpdateAndRender -EXPORT:EngineSetPlatform -PDB:engine_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%.pdb -incremental:no -opt:ref
cl %commonCompilerFlags% -DGLEW_STATIC ..\src\platform\win32_main.cpp /link %commonLinkerFlags%


    :: mingw g++ compiler commands (doesn't generate debug symbols)
::
::set includes=-Isrc -Itrdp/GLEW/include -Itrdp/GLFW/include
::set libs=-Ltrdp/GLFW/lib -Ltrdp/GLEW/lib -lglfw3 -Wl,-Bstatic -lglew32s -Wl,--as-needed -lopengl32
::
::g++ -g -s -shared src/engine.cpp %includes% -o bin/candle.dll
::g++ -g src/platform/win32_main.cpp -DGLEW_STATIC %includes% %libs% -o bin/windows.exe
::

popd