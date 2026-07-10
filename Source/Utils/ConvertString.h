#pragma once
#include <Windows.h>
#include <string>

/// <summary>
/// UTF-8文字列(std::string)をワイド文字列(std::wstring)へ変換する
/// </summary>
/// <param name="str">
/// UTF-8形式の文字列
/// </param>
/// <returns>
/// UTF-16形式へ変換したワイド文字列
/// </returns>
/// <remarks>
/// Windows APIではUTF-16文字列を要求する関数が多いため、
/// APIへ渡す前の変換処理として利用する。
/// </remarks>
inline std::wstring ConvertString(const std::string &str) {
	if (str.empty()) {
		return std::wstring();
	}

	auto sizeNeeded = MultiByteToWideChar(
		CP_UTF8, 0,
		reinterpret_cast<const char *>(&str[0]),
		static_cast<int>(str.size()),
		NULL, 0);

	if (sizeNeeded == 0) {
		return std::wstring();
	}

	std::wstring result(sizeNeeded, 0);

	MultiByteToWideChar(
		CP_UTF8, 0,
		reinterpret_cast<const char *>(&str[0]),
		static_cast<int>(str.size()),
		&result[0], sizeNeeded);

	return result;
}

/// <summary>
/// ワイド文字列(std::wstring)をUTF-8文字列(std::string)へ変換する
/// </summary>
/// <param name="str">
/// UTF-16形式のワイド文字列
/// </param>
/// <returns>
/// UTF-8形式へ変換した文字列
/// </returns>
/// <remarks>
/// ログ出力やファイル保存など、std::stringが必要な場面で使用する。
/// </remarks>
inline std::string ConvertString(const std::wstring &str) {
	if (str.empty()) {
		return std::string();
	}

	auto sizeNeeded = WideCharToMultiByte(
		CP_UTF8, 0,
		str.data(),
		static_cast<int>(str.size()),
		NULL, 0, NULL, NULL);

	if (sizeNeeded == 0) {
		return std::string();
	}

	std::string result(sizeNeeded, 0);

	WideCharToMultiByte(
		CP_UTF8, 0,
		str.data(),
		static_cast<int>(str.size()),
		result.data(),
		sizeNeeded,
		NULL, NULL);

	return result;
}