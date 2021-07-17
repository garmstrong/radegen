#include <string>
#include <iostream>
#include <map>
#include <sys/stat.h>
#include "osutils.h"

#ifdef __ANDROID__
#include <android/log.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
struct android_app*  g_App = NULL;
#endif

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#include <tchar.h>
#define getcwd _getcwd
#elif __linux__
#include <unistd.h>
#include <dirent.h>
#include <uuid/uuid.h>
#include <climits>
#endif

namespace rade
{
    std::string DefaultTextureFileName()
    {
        return "notexture.png";
    }

    std::chrono::high_resolution_clock::time_point GetTimeCount()
    {
        return std::chrono::high_resolution_clock::now();
    }

    std::string GetWorkingDir()
    {
#ifdef __ANDROID__
        return g_App->activity->externalDataPath;
#else
        char buffer[4096];
        char* workdir = getcwd(buffer, sizeof(buffer));
        std::string s_cwd;
        if (workdir)
        {
            s_cwd = workdir;
        }
        return s_cwd;
#endif
    }

    std::string GetDateTime()
    {
        time_t now = time(nullptr);
        struct tm tstruct = *localtime(&now);
        char buf[80];
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
        return buf;
    }

    bool DirectoryExists(const std::string& path)
    {
#if defined(_WIN32)
        struct _stat info;
        if (_stat(path.c_str(), &info) != 0)
        {
            return false;
        }
        return (info.st_mode & _S_IFDIR) != 0;
#else
        struct stat info;
        if (stat(path.c_str(), &info) != 0)
        {
            return false;
        }
        return (info.st_mode & S_IFDIR) != 0;
#endif
    }

    bool CreateDirectories(const std::string& path)
    {
#if defined(_WIN32)
        int ret = _mkdir(path.c_str());
#else
        mode_t mode = 0755;
        int ret = mkdir(path.c_str(), mode);
#endif
        if (ret == 0)
            return true;

        switch (errno)
        {
        case ENOENT:
            // parent didn't exist, try to create it
        {
            size_t pos = path.find_last_of('/');
            if (pos == std::string::npos)
#if defined(_WIN32)
                pos = path.find_last_of('\\');
                if (pos == std::string::npos)
#endif
                return false;
            if (!rade::CreateDirectories(path.substr(0, pos)))
                return false;
        }
            // now, try to create again
#if defined(_WIN32)
            return 0 == _mkdir(path.c_str());
#else
            return 0 == mkdir(path.c_str(), mode);
#endif

        case EEXIST:
            // done!
            return rade::DirectoryExists(path);

        default:
            return false;
        }
    }

    bool FileExists(const std::string& filename, int* size)
    {
        struct stat st{};
        bool success = stat(filename.c_str(), &st) == 0;
        if (size != nullptr)
        {
            *size = st.st_size;
        }
        return success;
    }

    bool RemoveFile(const char* filename)
    {
        bool success;
        if (remove(filename) != 0)
        {
            success = false;
            Log("Failed to removed file %s\n", filename);
        }
        else
        {
            success = true;
            Log("Removed file %s\n", filename);
        }
        return success;
    }

    char* ReadFile(const std::string& filename, long* size)
    {
        FILE* fp = fopen(filename.c_str(), "rb");
        if (fp == nullptr)
        {
            Log("Could not open file: %s\n", filename.c_str());
            return nullptr;
        }

        // Get size of file
        fseek(fp, 0, SEEK_END);
        long fileSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char* data = (char*)malloc(fileSize);
        memset(data, '\0', fileSize);
        fread(data, 1, fileSize, fp);
        //data[fileSize + 1] = '\0';
        fclose(fp);

        *size = fileSize;
        return data;
    }


    bool GetFilesInDir(const std::string& path,
                       std::vector<std::string>& files,
                       bool returnFiles,
                       bool returnDirectories)
    {
#ifdef _WIN32
        HANDLE hFind;
        WIN32_FIND_DATA wfd;
        TCHAR GeneralPath[0xFF];
        //TCHAR AgainFolder[0xFF];
        //TCHAR FileFullPath[0xFF];

        _stprintf(GeneralPath, _T("%s\\*.*"), path.c_str());
        hFind = FindFirstFile(GeneralPath, &wfd);

        if(INVALID_HANDLE_VALUE==hFind)
            return false;

        do
        {
            bool isDir = wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

            if( isDir && returnDirectories )
            {
                //if( !_tcscmp(wfd.cFileName, _T(".")) || !_tcscmp(wfd.cFileName, _T("..")) )
                    //continue;
                files.emplace_back(wfd.cFileName);
            }
            else
            {
                //std::string fullPath = path.c_str() + std::string("\\") + wfd.cFileName;
                //_stprintf(FileFullPath, _T("%s\\%s"), path.c_str(), wfd.cFileName); //  "Folder\\fileName.extension"
                //_tprintf(_T("%s\n"),FileFullPath);
                if(returnFiles)
                    files.emplace_back(wfd.cFileName);
            }

        }while(FindNextFile(hFind, &wfd));

        //CloseHandle(hFind);
        hFind=INVALID_HANDLE_VALUE;
        return true;
#else
        bool retVal = true;

        DIR* dir;
        struct dirent* ent;
        if ((dir = opendir(path.c_str())) != nullptr)
        {
            // print all the files and directories within directory
            while ((ent = readdir(dir)) != nullptr)
            {
                std::string fileName(ent->d_name);

                bool isFile = fileName != "." && fileName != ".." && ent->d_type == 8;
                if (!isFile && returnDirectories || isFile && returnFiles)
                {
                    files.push_back(fileName);
                }
            }
            closedir(dir);
        }
        else
        {
            // could not open directory
            retVal = false;
        }
        return retVal;
#endif
    }

    char* ReadPlatformAssetFile(const char* filename, long* size)
    {
#ifdef __ANDROID__
        void *data = nullptr;
        long num_bytes = GetAssetData(filename, &data);
        *size = num_bytes;
        return (char*)data;
#else
        return rade::ReadFile(filename, size);
#endif
    }

    void Log(const char* pszFormat, ...)
    {
        static char szBuff[512];
        va_list vArgs;
        va_start(vArgs, pszFormat);
        vsprintf(szBuff, pszFormat, vArgs);
        va_end(vArgs);
        std::cout << szBuff;
        std::cout << std::flush;

#ifdef __ANDROID__
        const char *APPNAME = "TRAILSONAR";
        __android_log_write(ANDROID_LOG_VERBOSE, APPNAME, szBuff);
#endif

    }

    void LogOnce(const char* pszFormat, ...)
    {
        static std::map<std::size_t, size_t> existingLogs;

        static char szBuff[512];
        va_list vArgs;
        va_start(vArgs, pszFormat);
        vsprintf(szBuff, pszFormat, vArgs);
        va_end(vArgs);

        std::size_t hashID = std::hash<std::string>{}(szBuff);

        if (existingLogs.find(hashID) == existingLogs.end())
        {
            std::cout << szBuff;
            //std::cout << std::flush;
            existingLogs[hashID] = existingLogs[hashID] + 1;
        }
    }

    void Assert(bool condition, const char* pszFormat, ...)
    {
        if (!condition)
        {
            static char szBuff[512];
            va_list vArgs;
            va_start(vArgs, pszFormat);
            vsprintf(szBuff, pszFormat, vArgs);
            va_end(vArgs);
            rade::Log(szBuff);
            exit(0);
        }
    }

    void Abort(const char* pszFormat, ...)
    {
        rade::Assert(false, pszFormat);
    }

    void Abort(const std::string& str)
    {
        rade::Assert(false, str.c_str());
    }

    std::string ResourcePath(const std::string& resourceName)
    {
        return std::string("data/" + resourceName);
    }

    std::string TexturePath(const std::string& resourceName)
    {
        return std::string("data/textures/" + resourceName);
    }

    void Warning(const char* pszFormat, ...)
    {
        rade::Log(pszFormat);
    }

    uint32_t HashString(const char* s)
    {
        uint32_t hash = 0;

        for (; *s; ++s)
        {
            hash += *s;
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }

        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);

        return hash;
    }

#ifdef __ANDROID__
    // Helper to retrieve data placed into the assets/ directory (android/app/src/main/assets)
    long GetAssetData(const char* filename, void** outData)
    {
        long num_bytes = 0;
        AAsset* asset_descriptor = AAssetManager_open(g_App->activity->assetManager, filename, AASSET_MODE_BUFFER);
        if (asset_descriptor)
        {
            num_bytes = AAsset_getLength(asset_descriptor);
            *outData = malloc(num_bytes);
            int64_t num_bytes_read = AAsset_read(asset_descriptor, *outData, num_bytes);
            AAsset_close(asset_descriptor);
            Assert(num_bytes_read == num_bytes, "bytes mismatch on AAsset_read() for %s\n", filename);
        }
        return num_bytes;
    }
#endif

//    void GenerateGUID(unsigned char* out_bytes)
//    {
//#ifdef _WIN32
//        GUID newId;
//        CoCreateGuid(&newId);
//
//        unsigned char bytes[16];
//        ::CopyMemory(&bytes, &newId, sizeof(bytes));
//        out_bytes.assign(bytes, bytes + 16);
//#else
//        uuid_t id;
//        uuid_generate(id);
//        rade::Assert(sizeof(uuid_t) == 128/CHAR_BIT, "GUID size mis-match\n");
//        memcpy(out_bytes, id, 16);
//#endif
//    }
};
