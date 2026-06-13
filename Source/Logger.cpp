#include "Logger.h"

#pragma comment(lib, "Dbghelp.lib")

#include <DbgHelp.h>
#include <filesystem>
#include <strsafe.h>
#include <chrono>
#include <format>

void Logger::Initialize() {
    // ログをファイル出力
    std::filesystem::create_directory("logs");	// ログのディレクトリを用意

    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();	// 現在時刻を取得(UTC時刻)
    std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>		// ログファイルの名前にコンマ何秒はいらないので、削って秒にする
        nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
    std::chrono::zoned_time localTime{ std::chrono::current_zone(), nowSeconds };	// 日本時間(PCの設定時間)に変換
    std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);			// formatを使って年月日_時分秒の文字列に変換
    std::string logFilePath = std::string("logs/") + dateString + ".log";			// 時刻を使ってファイル名を決定

    stream_.open(logFilePath);

    // CrashHandlerの登録
    SetUnhandledExceptionFilter(Logger::ExportDump);
}

void Logger::Finalize() {
    if (stream_.is_open()) {
        stream_.close();
    }
}

void Logger::Log(const std::string& message) {
    stream_ << message << std::endl;
    OutputDebugStringA(message.c_str());
}


LONG WINAPI Logger::ExportDump(EXCEPTION_POINTERS *exception) {
	// Dumpsディレクトリを用意
	CreateDirectory(L"./Dumps", nullptr);

	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };

	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);

	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	
    if (dumpFileHandle != INVALID_HANDLE_VALUE) {
        // processId(このexeのID)とクラッシュ(例外)の発生したthreadIdを取得
        DWORD processId = GetCurrentProcessId();
        DWORD threadId = GetCurrentThreadId();
        // 設定情報を入力
        MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
        minidumpInformation.ThreadId = threadId;
        minidumpInformation.ExceptionPointers = exception;
        minidumpInformation.ClientPointers = TRUE;

        // Dumpを出力。MiniDumpNormalは最低限の情報を出力するフラグ
        MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle,
            MiniDumpNormal, &minidumpInformation, nullptr, nullptr);

        CloseHandle(dumpFileHandle);
    }

	// 他に関連づけられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する
	return EXCEPTION_EXECUTE_HANDLER;
}