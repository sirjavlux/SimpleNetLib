#include "stdafx.h"
#include <tge/error/ErrorManager.h>
#include <tge/windows/WindowsWindow.h>
#include <tge/drawers/DebugDrawer.h>
#include <iostream>
#include <cstdarg>
using namespace Tga;


#define CONSOLE_TEXT_COLOR_RED 12
#define CONSOLE_TEXT_COLOR_YELLOW 14
#define CONSOLE_TEXT_COLOR_GREEN 10
#define CONSOLE_TEXT_COLOR_WHITE 15

inline uint32_t jenkins_one_at_a_time_hash(const uint8_t* key, size_t length) {
	size_t i = 0;
	uint32_t hash = 0;
	while (i != length) {
		hash += key[i++];
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}

ErrorManager::ErrorManager()
	:myErrorsReported(0)
{
}


ErrorManager::~ErrorManager(void)
{
}


void Tga::ErrorManager::AddLogListener(callback_function_log aFunctionToCall, callback_function_error aFunctionToCallOnError)
{
	if (aFunctionToCall)
	{
		myLogFunctions.push_back(aFunctionToCall);
	}
	if (aFunctionToCallOnError)
	{
		myErrorFunctions.push_back(aFunctionToCallOnError);
	}
}

std::string string_vsprintf(const char* format, std::va_list args)
{
	va_list tmp_args; //unfortunately you cannot consume a va_list twice
	va_copy(tmp_args, args); //so we have to copy it
	const int required_len = _vscprintf(format, tmp_args) + 1;
	va_end(tmp_args);

	char buff[4096*4];
	memset(buff, 0, required_len);
	if (vsnprintf_s(buff, required_len, format, args) < 0)
	{
		return "string_vsprintf encoding error";
	}
	return std::string(buff);
}

void Tga::ErrorManager::ErrorPrint(const char* aFile, int aline, const char* aFormat, ...)
{
	SetConsoleColor(CONSOLE_TEXT_COLOR_RED);
	std::string file = std::string(aFile);
	const size_t last_slash_idx = file.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
	{
		file.erase(0, last_slash_idx + 1);
	}

	va_list argptr;
	va_start(argptr, aFormat);
	
	std::string str{ string_vsprintf(aFormat, argptr) };

	uint32_t hashed = jenkins_one_at_a_time_hash((uint8_t*)str.c_str(), str.size());
	if (myPrintedErrors.find(hashed) != myPrintedErrors.end())
	{
		return;
	}
	printf("Error in %s at line:%i ", file.c_str(), aline);
	myPrintedErrors[hashed] = true;
	for (unsigned int i = 0; i < myErrorFunctions.size(); i++)
	{
		myErrorFunctions[i](str);
	}
	OutputDebugStringA(str.c_str());
	vfprintf(stderr, aFormat, argptr);
	va_end(argptr);
	std::cout << std::endl;
	SetConsoleColor(CONSOLE_TEXT_COLOR_WHITE);
	myErrorsReported++;

	Engine::GetInstance()->GetDebugDrawer().ShowErrorImage();
}


void Tga::ErrorManager::InfoPrint( const char* aFormat, ... )
{
	SetConsoleColor(CONSOLE_TEXT_COLOR_GREEN);
	va_list argptr;
	va_start(argptr, aFormat);

	std::string str{ string_vsprintf(aFormat, argptr) };
	for (unsigned int i = 0; i < myLogFunctions.size(); i++)
	{
		myLogFunctions[i](str);
	}

	vfprintf(stderr, aFormat, argptr);
	va_end(argptr);
	std::cout << std::endl;
	SetConsoleColor(CONSOLE_TEXT_COLOR_WHITE);
}



void Tga::ErrorManager::InfoTip(const char* aFormat, ...)
{
	SetConsoleColor(CONSOLE_TEXT_COLOR_YELLOW);
	va_list argptr;
	va_start(argptr, aFormat);

	std::string str{ string_vsprintf(aFormat, argptr) };
	for (unsigned int i = 0; i < myLogFunctions.size(); i++)
	{
		myLogFunctions[i](str);
	}

	vfprintf(stderr, aFormat, argptr);
	va_end(argptr);
	std::cout << std::endl;
	SetConsoleColor(CONSOLE_TEXT_COLOR_WHITE);
}

void Tga::ErrorManager::SetConsoleColor(int aColor)
{
	HANDLE  hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!hConsole)
	{
		return;
	}
	SetConsoleTextAttribute(hConsole, static_cast<WORD>(aColor));
}

