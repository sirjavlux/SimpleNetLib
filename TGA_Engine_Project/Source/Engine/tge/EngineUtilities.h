#pragma once

namespace Tga {
	static std::wstring ToWstring(const std::string& s) {
		return std::wstring(s.begin(), s.end());
	}

	static std::string FromWstring(const std::wstring& ws) {
		std::string s;
		size_t size;
		s.resize(ws.length());
		wcstombs_s(&size, &s[0], s.size() + 1, ws.c_str(), ws.size());
		return s;
	}
}