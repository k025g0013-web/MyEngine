#pragma once
#include <string>

std::wstring ConvertString(const std::string& str);	// string -> wstring
std::string ConvertString(const std::wstring& str);	// wstring -> string