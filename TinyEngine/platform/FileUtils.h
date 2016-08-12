
#ifndef __ENGINE_FILE_UTILS_H__
#define __ENGINE_FILE_UTILS_H__

#include "platform/Common.h"
#include <string>

using namespace std;

NS_ENGINE_BEGIN

class FileUtils
{
public:
	virtual ~FileUtils(){}

	static FileUtils* sharedFileUtils();

	static void purgeFileUtils();

	virtual bool isFileExist(const char* fName) = 0;

	virtual const char* getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize) = 0;

	virtual string getFileMD5(const char* pszFileName);
    
    
    
	virtual std::string getDataPath() = 0;
    
    virtual std::string getPersistentDataPath() = 0;
    
    virtual std::string getTemporaryCachePath() = 0;

	virtual std::string getAppPath() = 0;

	virtual std::string getExecuteFilePath() = 0;
    

protected:
	FileUtils(){}

	static FileUtils* s_sharedFileUtils;
};


NS_ENGINE_END

#endif
