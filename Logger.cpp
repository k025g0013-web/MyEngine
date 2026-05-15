#include "Logger.h"

#include <Windows.h>

#include <filesystem>
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