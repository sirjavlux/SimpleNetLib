#pragma once

#include <iostream>
#include <string>
#include <Windows.h>

namespace StringUtility
{
	inline std::wstring StringToWideString(const std::string& InStr) {
	// Get the required buffer size
	const int wideStrLen = MultiByteToWideChar(CP_UTF8, 0, InStr.c_str(), -1, NULL, 0);
	if (wideStrLen == 0) {
		// Error handling
		std::cerr << "Error in MultiByteToWideChar: " << GetLastError() << std::endl;
		return L"";
	}

	// Allocate buffer for wide string
	wchar_t* wideStr = new wchar_t[wideStrLen];
	if (wideStr == nullptr) {
		// Error handling
		std::cerr << "Memory allocation failed" << std::endl;
		return L"";
	}

	// Convert multibyte string to wide string
	if (MultiByteToWideChar(CP_UTF8, 0, InStr.c_str(), -1, wideStr, wideStrLen) == 0) {
		// Error handling
		std::cerr << "Error in MultiByteToWideChar: " << GetLastError() << std::endl;
		delete[] wideStr;
		return L"";
	}

	std::wstring wideString(wideStr);

	// Cleanup
	delete[] wideStr;

	return wideString;
}
}