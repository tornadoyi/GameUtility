
#ifndef __FILE_UTILS_IOS_H__
#define __FILE_UTILS_IOS_H__

#include "platform/FileUtils.h"

NS_ENGINE_BEGIN

class FileUtilsIOS : public FileUtils
{
public:
    FileUtilsIOS();
    
    virtual ~FileUtilsIOS();
    
    virtual bool isFileExist(const char* fileName);
    
    virtual const char* getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize);
    
    virtual std::string getDataPath();
    
    virtual std::string getPersistentDataPath();
    
    virtual std::string getTemporaryCachePath();
    
    virtual std::string getAppPath();
    
    virtual std::string getExecuteFilePath();


protected:
    std::string searchPathInDomain(NSSearchPathDirectory directory);
    
    
private:
    std::string _appPath;
    std::string _documentPath;
    std::string _cachePath;
    
};

NS_ENGINE_END


#endif