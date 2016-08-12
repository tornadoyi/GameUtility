/*
 * by Tank. 2014-11-30
 */
#ifndef __FILE_UTILS_ANDROID_H__
#define __FILE_UTILS_ANDROID_H__

#include "platform/FileUtils.h"
#include <string>

struct zip;

NS_ENGINE_BEGIN

class FileUtilsAndroid : public FileUtils
{
public:
	FileUtilsAndroid();

	virtual ~FileUtilsAndroid();

	virtual bool isFileExist(const char* fileName);

	virtual const char* getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize);
	
	virtual std::string getDataPath();

	virtual std::string getPersistentDataPath();

	virtual std::string getTemporaryCachePath();

	virtual std::string getAppPath();

	virtual std::string getExecuteFilePath();


protected:
	std::string getApkPath();

	zip* openZipFile(const char* fileName);

	void closeZipFile(zip* zipFile);

protected:
	zip* _apkFile;

	std::string _apkPath;

	std::string _filesPath;

	std::string _cachePath;
};

NS_ENGINE_END

#endif
