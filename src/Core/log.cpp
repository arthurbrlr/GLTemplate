#include "core.h"
#include <stdio.h>
#include <stdarg.h>

internal void LogMsg(const char* msg, ...)
{
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(stdout, msg, argptr);
    va_end(argptr);
}

internal void LogInfo(const char* msg, ...)
{
    printf("\033[1;32m");
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(stdout, msg, argptr);
    va_end(argptr);
    printf("\033[0m");
}

internal void LogWarn(const char* msg, ...)
{
    printf("\033[1;33m");
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(stdout, msg, argptr);
    va_end(argptr);
    printf("\033[0m");
}

internal void LogErr(const char* msg, ...)
{
    printf("\033[1;31m");
    va_list argptr;
    va_start(argptr, msg);
    vfprintf(stdout, msg, argptr);
    va_end(argptr);
    printf("\033[0m");
}