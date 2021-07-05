#include <string>
#include <iostream>
#include <map>
#include "osutils.h"
#include <sys/stat.h>

#ifdef __ANDROID__
#include <android/log.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
struct android_app*  g_App = NULL;
#endif

#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#elif __linux__

#include <unistd.h>

#endif


std::chrono::high_resolution_clock::time_point OS::GetTimeCount()
{
    return std::chrono::high_resolution_clock::now();
}

std::string OS::GetWorkingDir()
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

std::string OS::GetDateTime()
{
    time_t now = time(nullptr);
    struct tm tstruct = *localtime(&now);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

bool OS::DirectoryExists(const std::string& path)
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

bool OS::CreateDirectories(const std::string& path)
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
        int pos = path.find_last_of('/');
        if (pos == std::string::npos)
#if defined(_WIN32)
            pos = path.find_last_of('\\');
            if (pos == std::string::npos)
#endif
            return false;
        if (!CreateDirectories(path.substr(0, pos)))
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
        return DirectoryExists(path);

    default:
        return false;
    }
}

bool OS::FileExists(const std::string& filename, int* size)
{
    struct stat st {};
    bool success = stat(filename.c_str(), &st) == 0;
    if (size != nullptr)
    {
        *size = st.st_size;
    }
    return success;
}

bool OS::RemoveFile(const char* filename)
{
    bool success;
    if( remove( filename ) != 0 )
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

char* OS::ReadFile(const std::string& filename, long* size)
{
    FILE* fp = fopen(filename.c_str(), "r");
    if (fp == nullptr)
    {
        Log("Could not open file: %s\n", filename.c_str());
        return nullptr;
    }

    // Get size of file
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* data = (char*)malloc(fileSize + 1);
    memset(data, '\0', fileSize + 1);
    fread(data, 1, fileSize, fp);
    fclose(fp);

    *size = fileSize;
    return data;
}

char* OS::ReadPlatformAssetFile(const char* filename, long* size)
{
#ifdef __ANDROID__
    void *data = nullptr;
    long num_bytes = GetAssetData(filename, &data);
    *size = num_bytes;
    return (char*)data;
#else
    return ReadFile(filename, size);
#endif
}

void OS::Log(const char* pszFormat, ...)
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

void OS::LogOnce(const char* pszFormat, ...)
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

void OS::Assert(bool condition, const char* pszFormat, ...)
{
    if (!condition)
    {
        static char szBuff[512];
        va_list vArgs;
        va_start(vArgs, pszFormat);
        vsprintf(szBuff, pszFormat, vArgs);
        va_end(vArgs);
        Log(szBuff);
        exit(0);
    }
}

void OS::Abort(const char* pszFormat, ...)
{
    Assert(false, pszFormat);
}

void OS::Abort(const std::string& str)
{
    Assert(false, str.c_str());
}

std::string OS::ResourcePath(const std::string& resourceName)
{
    return std::string("data/" + resourceName);
}

std::string OS::TexturePath(const std::string& resourceName)
{
    return std::string("data/textures/" + resourceName);
}

void OS::Warning(const char* pszFormat, ...)
{
    Log(pszFormat);
}

uint32_t OS::HashString(const char * s)
{
    uint32_t hash = 0;

    for(; *s; ++s)
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
long OS::GetAssetData(const char* filename, void** outData)
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

