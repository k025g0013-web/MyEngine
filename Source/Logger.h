#pragma once

#include <windows.h>
#include <fstream>
#include <string>

class Logger {
public:
    void Initialize();
    void Finalize();

    // ログ書き込み
    void Log(const std::string& message);

private:
    // CrashHandler
    static LONG WINAPI ExportDump(EXCEPTION_POINTERS *exception);

private:
    std::ofstream stream_;
};