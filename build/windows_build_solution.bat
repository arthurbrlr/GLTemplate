@echo off
pushd %~dp0\..\
call build\windows\premake5.exe vs2019
popd
PAUSE
