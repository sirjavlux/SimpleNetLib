#pragma once
#include <functional>
#include <thread>
#include <mutex>
#include <filesystem>
namespace fs = std::filesystem;
namespace Tga
{
	typedef std::function<void(const std::wstring&)> callback_function_file;
	class FileWatcher
	{
	public:
		FileWatcher();
		~FileWatcher();

		/* Will check the file for includes and add them as well*/
		bool WatchFileChange(std::wstring aFile, callback_function_file aFunctionToCallOnChange);

		void FlushChanges();
	private:
		void UpdateChanges();
		void CheckFileChages(const fs::path& aFile, long long aTimeStampLastChanged);
		void OnFileChange(const fs::path& aFile);
		std::thread* myThread;

		typedef std::map<std::wstring, std::vector<WIN32_FIND_DATA>> FolderMap;
		FolderMap myFolders;

		std::vector<fs::path> myFileChangedThreaded;
		std::vector<fs::path> myFileChanged;
		std::map<std::wstring, std::vector<callback_function_file> > myCallbacks;
		std::map<fs::path, long long> myThreadedFilesToWatch;

		std::mutex myMutex;
		std::mutex myAddNewFolderMutex;
		bool myShouldEndThread;
		bool myThreadIsDone;
	};

}