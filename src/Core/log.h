#pragma once
#include "core.h"

internal void LogMsg(const char* msg, ...); // normal
internal void LogInfo(const char* msg, ...); // bold green
internal void LogWarn(const char* msg, ...); // bold yellow
internal void LogErr(const char* msg, ...); // bold red