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

class OS
{
public:

    OS()
    = default;

    virtual ~OS()
    = default;

    static const std::string DefaultTextureFileName()
    {
        return "notexture.png";
    }

    static std::string TexturePath(const std::string& resourceName);

    static std::string GetWorkingDir();

    static std::string GetDateTime();

    static bool CreateDirectories(const std::string& path);

    static bool DirectoryExists(const std::string& path);

    static bool FileExists(const std::string& filename, int* size = nullptr);

    static bool RemoveFile(const char* filename);

    static char* ReadFile(const std::string& filename, long* size);

    static bool GetFilesInDir(const std::string& path, std::vector<std::string>& files, bool returnFiles, bool returnDirectories);

    static char* ReadPlatformAssetFile(const char* filename, long* size);

    static void Log(const char* pszFormat, ...);

    static void LogOnce(const char* pszFormat, ...);

    static void Assert(bool condition, const char* pszFormat, ...);

    static void Abort(const std::string& str);

    static void Abort(const char* pszFormat, ...);

    static std::string ResourcePath(const std::string& resourceName);

    static std::chrono::high_resolution_clock::time_point GetTimeCount();

    static void Warning(const char* pszFormat, ...);

    uint32_t HashString(const char* s);

#ifdef __ANDROID__
    static long GetAssetData(const char* filename, void** outData);

#endif

//	// must be implemented by platform specific classes
//	virtual void GenerateGUID(std::vector<unsigned char> & out_bytes) = 0;
//	virtual void SetWorkingDir(const std::string &path) = 0;
//	virtual bool GetFilesInDir(const std::string &path, std::vector<std::string> &files) = 0;

private:

};

#ifdef __ANDROID__
extern struct android_app*  g_App;
#endif
