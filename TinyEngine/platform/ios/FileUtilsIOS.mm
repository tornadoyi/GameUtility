

#import <Foundation/Foundation.h>

#include <string>
#include "FileUtilsIOS.h"

using namespace std;

NS_ENGINE_BEGIN

FileUtils* FileUtils::sharedFileUtils()
{
    if (s_sharedFileUtils == NULL)
    {
        s_sharedFileUtils = new FileUtilsIOS();
    }
    return s_sharedFileUtils;
}

FileUtilsIOS::FileUtilsIOS()
{
    
}


FileUtilsIOS::~FileUtilsIOS()
{
    
}


bool FileUtilsIOS::isFileExist(const char* fileName)
{
    if (NULL == fileName || strlen(fileName) == 0)
    {
        return false;
    }
    
    std::string strFilePath(fileName);
    bool bRet = false;
    
    if (fileName[0] != '/')
    {
        std::string path;
        std::string file;
        size_t pos = strFilePath.find_last_of("/");
        if (pos != std::string::npos)
        {
            file = strFilePath.substr(pos+1);
            path = strFilePath.substr(0, pos+1);
        }
        else
        {
            file = strFilePath;
        }
        
        NSString* fullpath = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:file.c_str()]
                                                             ofType:nil
                                                        inDirectory:[NSString stringWithUTF8String:path.c_str()]];
        if (fullpath != nil) {
            bRet = true;
        }
    }
    else
    {
        // Search path is an absolute path.
        NSFileManager* s_fileManager = [NSFileManager defaultManager];
        if ([s_fileManager fileExistsAtPath:[NSString stringWithUTF8String:strFilePath.c_str()]]) {
            bRet = true;
        }
    }
    
    return bRet;
}


const char* FileUtilsIOS::getFileData(const char* fileName, const char* pszMode, unsigned long * pSize)
{
    if (NULL == fileName || strlen(fileName) == 0)
    {
        return NULL;
    }
    
    NSString* fullpath = [NSString stringWithUTF8String:fileName];
    std::string strFilePath(fileName);
    bool bRet = false;
    
    if (fileName[0] != '/')
    {
        std::string path;
        std::string file;
        size_t pos = strFilePath.find_last_of("/");
        if (pos != std::string::npos)
        {
            file = strFilePath.substr(pos+1);
            path = strFilePath.substr(0, pos+1);
        }
        else
        {
            file = strFilePath;
        }
        
        fullpath = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:file.c_str()]
                                                             ofType:nil
                                                        inDirectory:[NSString stringWithUTF8String:path.c_str()]];
        if (fullpath != nil) {
            bRet = true;
        }
    }
    else
    {
        // Search path is an absolute path.
        NSFileManager* s_fileManager = [NSFileManager defaultManager];
        if ([s_fileManager fileExistsAtPath:[NSString stringWithUTF8String:strFilePath.c_str()]]) {
            bRet = true;
        }
    }
    
    if(!bRet) return NULL;
    
    unsigned char * pBuffer = NULL;
    *pSize = 0;
    do
    {
        // read the file from hardware
        FILE *fp = fopen([fullpath UTF8String], pszMode);
        BREAK_IF(!fp);
        
        fseek(fp,0,SEEK_END);
        *pSize = ftell(fp);
        fseek(fp,0,SEEK_SET);
        pBuffer = new unsigned char[*pSize];
        *pSize = fread(pBuffer,sizeof(unsigned char), *pSize,fp);
        fclose(fp);
    } while (0);
    
    if (! pBuffer)
    {
        std::string msg = "Get data from file(";
        msg.append(fileName).append(") failed!");
        
        LOG("%s", msg.c_str());
    }
    return (const char*)pBuffer;
}


std::string FileUtilsIOS::getDataPath()
{
    NSString *appPath = [[[NSBundle mainBundle] executablePath] stringByDeletingLastPathComponent];
    NSString* dataPath = [appPath stringByAppendingPathComponent:@"Data"];
    return[dataPath UTF8String];
}

std::string FileUtilsIOS::getPersistentDataPath()
{
    if(!_documentPath.empty()) return _documentPath;
    _documentPath = searchPathInDomain(NSDocumentDirectory);
    return _documentPath;
}

std::string FileUtilsIOS::getTemporaryCachePath()
{
    if(!_cachePath.empty()) return _cachePath;
    _cachePath = searchPathInDomain(NSCachesDirectory);
    return _cachePath;
}

std::string FileUtilsIOS::getExecuteFilePath()
{
    NSString *appPath = [[[NSBundle mainBundle] executablePath] stringByDeletingLastPathComponent];
    NSString *excutableName = [[NSBundle mainBundle] infoDictionary][@"CFBundleExecutable"];
    NSString *binaryPath = [appPath stringByAppendingPathComponent:excutableName];
    return [binaryPath UTF8String];
}

std::string FileUtilsIOS::getAppPath()
{
    if(!_appPath.empty()) return _appPath;
    NSString *appPath = [[[NSBundle mainBundle] executablePath] stringByDeletingLastPathComponent];
    _appPath = [appPath UTF8String];
    return _appPath;
}

std::string FileUtilsIOS::searchPathInDomain(NSSearchPathDirectory directory)
{
    NSArray* pathes= NSSearchPathForDirectoriesInDomains(directory, NSUserDomainMask, YES);
    NSString *path = [pathes objectAtIndex:0];
    return  [path UTF8String];

}



NS_ENGINE_END