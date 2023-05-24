#include "utils.hpp"

namespace utils {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converter;
}

std::string utils::ws2s(const std::wstring& wstr) {
	return converter.to_bytes(wstr);
}

std::wstring utils::s2ws(const std::string& wstr) {
	return converter.from_bytes(wstr);
}