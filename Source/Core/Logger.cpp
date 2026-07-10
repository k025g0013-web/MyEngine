#include "Logger.h"

#pragma comment(lib, "Dbghelp.lib")

#include <DbgHelp.h>
#include <filesystem>
#include <strsafe.h>
#include <chrono>
#include <format>

void Logger::Initialize() {
	// ログ保存用ディレクトリを作成する
	std::filesystem::create_directory("logs");

	// 現在時刻を取得する（UTC）
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

	// ファイル名には秒単位まで使用する
	auto nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);

	// ローカルタイムへ変換する
	std::chrono::zoned_time localTime{ std::chrono::current_zone(), nowSeconds };

	// 「YYYYMMDD_HHMMSS.log」の形式でログファイル名を作成する
	std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
	std::string logFilePath = "logs/" + dateString + ".log";

	stream_.open(logFilePath);

	// クラッシュ時にMiniDumpを生成できるよう例外ハンドラを登録する
	SetUnhandledExceptionFilter(Logger::ExportDump);
}

void Logger::Finalize() {
	if (stream_.is_open()) {
		// ログファイルを閉じる
		stream_.close();
	}
}

void Logger::Log(const std::string &message) {
	// ログファイルへ書き込む
	stream_ << message << std::endl;
	// デバッグ出力ウィンドウにも表示する
	OutputDebugStringA(message.c_str());
}

LONG WINAPI Logger::ExportDump(EXCEPTION_POINTERS *exception) {
	// Dumpファイル保存用ディレクトリを作成する
	CreateDirectory(L"./Dumps", nullptr);

	// 現在時刻を取得し、Dumpファイル名に利用する
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };

	// 日時を含むDumpファイル名を生成する
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);

	// Dump出力に必要な例外情報を設定する
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	if (dumpFileHandle != INVALID_HANDLE_VALUE) {
		// クラッシュ時の例外情報をMiniDumpへ出力するための設定を行う
		DWORD processId = GetCurrentProcessId();
		DWORD threadId = GetCurrentThreadId();

		MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{};
		minidumpInformation.ThreadId = threadId;
		minidumpInformation.ExceptionPointers = exception;
		minidumpInformation.ClientPointers = TRUE;

		// クラッシュ解析用のMiniDumpファイルを出力する
		MiniDumpWriteDump(
			GetCurrentProcess(),
			processId,
			dumpFileHandle,
			MiniDumpNormal,
			&minidumpInformation,
			nullptr,
			nullptr
		);

		// ファイルハンドルを閉じる
		CloseHandle(dumpFileHandle);
	}

	// 標準の例外処理へ制御を戻す
	return EXCEPTION_EXECUTE_HANDLER;
}