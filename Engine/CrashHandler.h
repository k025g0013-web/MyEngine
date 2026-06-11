#pragma once

#include <windows.h>

class CrashHandler {
public:
    static void Initialize();

private:
    static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);
};