#pragma once
#include <tge/Engine.h>
#include <map>
#include <vector>
#include <ostream>


namespace Tga
{

	class Log
	{
	public:
		template<typename... Args>
		static void WriteFormatted(std::ostream& someStream, const char* someFormat, const Args&... someArgs)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
			char buffer[1024];
			snprintf(buffer, 1024, someFormat, someArgs...);
			someStream << buffer << std::endl;
		}
	};

#define LOG(FORMAT, ...) Log::WriteFormatted(std::cout, FORMAT, __VA_ARGS__)
#define LOGV(FORMAT, ...) Log::WriteFormatted(std::cout, __FUNCTION__ ## ":" ## FORMAT, __VA_ARGS__)
#define LOGWARN(FORMAT ,...) Log::WriteFormatted(std::cout, "WARNING! " ## __FUNCTION__ ## ":"  ## FORMAT, __VA_ARGS__)
#define LOGERR(FORMAT, ...) Log::WriteFormatted(std::cerr, __FUNCTION__ ## ":"  ## FORMAT, __VA_ARGS__)

#define ASSERT(condition, message, ...) \
	{ \
		if(!condition) \
		{ \
			Log::WriteFormatted(std::cerr, "Assertion '" #condition "' failed in " ## __FUNCTION__ ## "! " #message); \
			std::terminate(); \
		} \
	}


#define LOGMESSAGE(TEXT) std::cout << __FUNCTION__ ## " at " ## __LINE__ ## " in " ## __FILE__ ## ": " << #TEXT << std::endl;

#ifndef _RETAIL
	#define SETCONSOLECOLOR(color) if (Tga::Engine::GetInstance()) Tga::Engine::GetInstance()->GetErrorManager().SetConsoleColor(color);
	#define ERROR_PRINT(aFormat, ...) if (Tga::Engine::GetInstance()) Tga::Engine::GetInstance()->GetErrorManager().ErrorPrint(__FILE__, __LINE__, aFormat, ##__VA_ARGS__);
    #define INFO_PRINT(aFormat, ...) if (Tga::Engine::GetInstance()) Tga::Engine::GetInstance()->GetErrorManager().InfoPrint(aFormat, ##__VA_ARGS__);
	#define INFO_TIP(aFormat, ...) if (Tga::Engine::GetInstance()) Tga::Engine::GetInstance()->GetErrorManager().InfoTip(aFormat, ##__VA_ARGS__);
#else
	#define SETCONSOLECOLOR(color);
	#define ERROR_PRINT(aFormat, ...);
	#define INFO_PRINT(aFormat, ...);
	#define INFO_TIP(aFormat, ...);
#endif
    class ErrorManager
    {
    public:
        typedef std::function<void(std::string)> callback_function_log;
		typedef std::function<void(std::string)> callback_function_error;
        ErrorManager();
        ~ErrorManager(void);
		void AddLogListener(callback_function_log aFunctionToCall, callback_function_error aFunctionToCallOnError);
        void Destroy();
        void ErrorPrint(const char* aFile, int aline, const char* aFormat, ...);
        void InfoPrint(const char* aFormat, ...);
		void InfoTip(const char* aFormat, ...);
		void SetConsoleColor(int aColor);
		unsigned int GetErrorsReported() const { return myErrorsReported; }
    private:
		std::vector<callback_function_log> myLogFunctions;
		std::vector<callback_function_error> myErrorFunctions;
		std::map<uint32_t, bool> myPrintedErrors;
		unsigned int myErrorsReported;
    };
}