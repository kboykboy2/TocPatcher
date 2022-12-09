#pragma once
#include <string>
#include"Source/Core/Serialization/IoStream.h"


namespace RFcore
{
	class System
	{
	public:
		static std::string WstrToStr(void* Wstr);
		static void* StrToWstr(std::string str);

		static std::string GetSavedGamesFolder();
		static std::string GetDocumentsFolder();
		static std::string GetAppDataFolder();
		static std::string GetCurrentExeDirectory();
		static std::string GetCurrentExePath();

		static std::string OpenFileDialog(bool& success);
		//static std::string SaveFileDialog(bool& success);
		static std::string OpenDirDialog(bool& success);

		static void  OutputFile(std::string path, char* file, uint64 size);
		static void  RemoveFile(std::string path);
		static bool  FileExists(std::string path);
		static char* GetFileData(std::string path, uint64* size);

		static std::vector<std::string> GetFilesInDirectory(std::string directory);

		static void Wait(int duration);
		static uint32 GetPid();
	};
}