#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <memory.h>
#include <ctime>
#include <memory.h>
#include <ctime>
#include <ratio>
#include <chrono>

namespace rade
{
    std::string DefaultTextureFileName();

    std::string TexturePath(const std::string& resourceName);

    std::string GetWorkingDir();

    std::string GetDateTime();

    bool CreateDirectories(const std::string& path);

    bool DirectoryExists(const std::string& path);

    bool FileExists(const std::string& filename, int* size = nullptr);

    bool RemoveFile(const char* filename);

    char* ReadFile(const std::string& filename, long* size);

    bool GetFilesInDir(const std::string& path, std::vector<std::string>& files, bool returnFiles,
            bool returnDirectories);

    char* ReadPlatformAssetFile(const char* filename, long* size);

    void Log(const char* pszFormat, ...);

    void LogOnce(const char* pszFormat, ...);

    void Assert(bool condition, const char* pszFormat, ...);

    void Abort(const std::string& str);

    void Abort(const char* pszFormat, ...);

    std::string ResourcePath(const std::string& resourceName);

    std::chrono::high_resolution_clock::time_point GetTimeCount();

    void Warning(const char* pszFormat, ...);

    uint32_t HashString(const char* s);

    //void GenerateGUID(unsigned char* out_bytes);

#ifdef __ANDROID__
    static long GetAssetData(const char* filename, void** outData);
    extern struct android_app*  g_App;
#endif

};
