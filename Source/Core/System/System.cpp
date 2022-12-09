#include"System.h"
#include"Source/Core/Error/Error.h"
#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>

std::string RFcore::System::WstrToStr(void* Wstr)
{
    std::wstring wString;
    wString.append(&((wchar_t*)Wstr)[0]);
    return std::string(wString.begin(), wString.end());
}

void* RFcore::System::StrToWstr(std::string str)
{
    const char* ctr = str.c_str();
    TCHAR* Wstr = new TCHAR[str.size()+1]{0};
    MultiByteToWideChar(CP_ACP, 0, ctr, str.size(), Wstr, str.size() +1);
    return Wstr;
}


std::string RFcore::System::GetSavedGamesFolder()
{
    PWSTR winPath; SHGetKnownFolderPath(FOLDERID_SavedGames, 0, NULL, &winPath);
    return RFcore::System::WstrToStr(winPath);
}

std::string RFcore::System::GetDocumentsFolder()
{
    PWSTR winPath; SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &winPath);
    return RFcore::System::WstrToStr(winPath);
}

std::string RFcore::System::GetAppDataFolder()
{
    PWSTR winPath; SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &winPath);
    return RFcore::System::WstrToStr(winPath);
}

std::string RFcore::System::GetCurrentExeDirectory()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");

    return std::string(buffer).substr(0, pos);
}

std::string RFcore::System::GetCurrentExePath()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);

    return std::string(buffer);
}

std::string RFcore::System::OpenFileDialog(bool& success)
{
    IFileOpenDialog* pFileOpen = nullptr;
    success = false;

    std::string path;
    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen))))
    {
        if (SUCCEEDED(pFileOpen->Show(NULL)))
        {
            IShellItem* pItem;
            if (SUCCEEDED(pFileOpen->GetResult(&pItem)))
            {
                PWSTR pszFilePath;
                // Display the file name to the user.
                if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath)))
                {
                    path = WstrToStr(pszFilePath);

                    CoTaskMemFree(pszFilePath);
                    success = true;
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }
    CoUninitialize();

    return path;
}

std::string RFcore::System::OpenDirDialog(bool& success)
{
    TCHAR path[MAX_PATH];
    std::string saved_path = "";
    const char* path_param = saved_path.c_str();

    BROWSEINFO bi = { 0 };
    bi.lpszTitle = (L"Browse for folder...");
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
    bi.lpfn = NULL;
    bi.lParam = (LPARAM)path_param;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    return "t";
}

void RFcore::System::OutputFile(std::string path, char* file, uint64 size)
{
    std::string Path = path;
    // fix path
    std::replace(Path.begin(), Path.end(), '/', '\\'); // replace / with \\
	// create directory
    std::string dir = Path.substr(0, Path.find_last_of('\\'));
    system((std::string("md \"" + dir) + "\"" + " command >nul 2>nul").c_str());

    IoStream io; io.OpenFile(path.c_str(), Write, Disk);
    io << Array(file, size);
    io.Close();
}

void RFcore::System::RemoveFile(std::string path)
{
    remove(path.c_str());
}

bool RFcore::System::FileExists(std::string path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

char* RFcore::System::GetFileData(std::string path, uint64* size)
{
    std::ifstream file(path, std::ios::binary);
    file.seekg(0, std::ios::end);
    *size = file.tellg();
    file.seekg(0, std::ios::beg);
    char* data = new char[*size];
    file.read(data, *size);
    return data;
}




std::vector<std::string> RFcore::System::GetFilesInDirectory(std::string directory)
{
    std::vector<std::string> files;
    WIN32_FIND_DATA ffd;
    HANDLE          hFind = INVALID_HANDLE_VALUE;
    TCHAR           szDir[MAX_PATH]{0};

    // Check that the input path plus 3 is not longer than MAX_PATH.
   // Three characters are for the "\*" plus NULL appended below.

    if (std::strlen(directory.c_str()) > (MAX_PATH - 3))
    {
        ThrowError("Directory path is too long");
    }

    // Prepare string for use with FindFile functions.  First, copy the
    // string to a buffer, then append '\*' to the directory name.

    MultiByteToWideChar(CP_ACP, 0, directory.c_str(), std::strlen(directory.c_str()), szDir, MAX_PATH);
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    // Find the first file in the directory.

    hFind = FindFirstFile(szDir, &ffd);

    // print dir
    /*
    for (int i = 0; i < MAX_PATH; i++)
    {
        std::cout << (char)szDir[i];
    }
    std::cout << '\n';
    */

    if (INVALID_HANDLE_VALUE != hFind)
    {
        do
        {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
            }
            else
            {
                std::string str = WstrToStr(ffd.cFileName);
                files.push_back(std::string(directory) + "\\" + str);
            }
        } while (FindNextFile(hFind, &ffd) != 0);
    }
    FindClose(hFind);
    
    return files;
}




void RFcore::System::Wait(int duration)
{
    Sleep(duration);
}


RFcore::uint32 RFcore::System::GetPid()
{
    return GetCurrentProcessId();
}