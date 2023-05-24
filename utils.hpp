#pragma once
#include <string>
#include <locale>
#include <codecvt>

namespace utils {
	std::string ws2s(const std::wstring& wstr);
	std::wstring s2ws(const std::string& str);
}