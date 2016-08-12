
#include "FileUtilsAndroid.h"
#include <string>
#include "support/libzip/zip.h"
#include "JVM.h"

using namespace std;

NS_ENGINE_BEGIN

FileUtils* tiny::FileUtils::sharedFileUtils()
{
	if (s_sharedFileUtils == NULL)
	{
		s_sharedFileUtils = new FileUtilsAndroid();
	}
	return s_sharedFileUtils;
}


FileUtilsAndroid::FileUtilsAndroid()
: _apkFile(NULL)
{
	std::string apkPath = getApkPath();
	_apkFile = openZipFile(apkPath.c_str());
	LOG("open android apk");
}


FileUtilsAndroid::~FileUtilsAndroid()
{
	if (_apkFile != NULL)
	{
		closeZipFile(_apkFile);
		_apkFile = NULL;
		LOG("close android apk");
	}
	
}

bool FileUtilsAndroid::isFileExist(const char* fileName)
{
    if (NULL == fileName || strlen(fileName) == 0)
    {
        return false;
    }

    bool bFound = false;
    
    // Check whether file exists in apk.
    if (fileName[0] != '/')
    {
		int fileIndex = zip_name_locate(_apkFile, fileName, 0);
		if (fileIndex != -1) bFound = true;
    }
    else
    {
        FILE *fp = fopen(fileName, "r");
        if(fp)
        {
            bFound = true;
            fclose(fp);
        }
    }
    return bFound;
}


const char* FileUtilsAndroid::getFileData(const char* fileName, const char* mode, unsigned long * pSize)
{
	if (NULL == fileName || strlen(fileName) == 0)
	{
		return NULL;
	}

	unsigned char * pData = NULL;

	if (fileName[0] != '/')
	{
		struct zip_stat fileStat;
		int fileIndex = zip_name_locate(_apkFile, fileName, 0);
		if (fileIndex != -1)
		{
			zip_file* file = zip_fopen_index(_apkFile, fileIndex, 0);
			if (file != NULL)
			{
				// get file stats
				zip_stat_init(&fileStat);
				zip_stat(_apkFile, fileName, 0, &fileStat);
				
				// read file
				pData = new unsigned char[fileStat.size];
				zip_fread(file, pData, fileStat.size);
				
				// close
				zip_fclose(file);

				if (pSize)
				{
					*pSize = fileStat.size;
				}
			}
		}
	}
	else
	{
		do
		{
			// read rrom other path than user set it
			//CCLOG("GETTING FILE ABSOLUTE DATA: %s", pszFileName);
			FILE *fp = fopen(fileName, mode);
			BREAK_IF(!fp);

			unsigned long size;
			fseek(fp, 0, SEEK_END);
			size = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			pData = new unsigned char[size];
			size = fread(pData, sizeof(unsigned char), size, fp);
			fclose(fp);

			if (pSize)
			{
				*pSize = size;
			}
		} while (0);
	}
	return (const char*)pData;
}

std::string FileUtilsAndroid::getDataPath()
{
	return getApkPath();
}

std::string FileUtilsAndroid::getPersistentDataPath()
{
	if (!_filesPath.empty()) return _filesPath;

	JNIEnv* env = JVM::getInstance()->getEnv();
	jobject activityObj = JVM::getInstance()->getUnityActivity();
	jclass activityClass = JVM::getInstance()->getUnityActivityClass();

	jmethodID getFilesDirMethodId = env->GetMethodID(activityClass, "getFilesDir", "()Ljava/io/File;");
	jobject fileObj = env->CallObjectMethod(activityObj, getFilesDirMethodId);
	jclass fileClass = env->GetObjectClass(fileObj);

	jmethodID getAbsolutePathMethod = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
	jobject pathObj = env->CallObjectMethod(fileObj, getAbsolutePathMethod);
	_filesPath = string(env->GetStringUTFChars((jstring)pathObj, 0));
	return _filesPath;
}

std::string FileUtilsAndroid::getTemporaryCachePath()
{
	if (!_cachePath.empty()) return _cachePath;

	JNIEnv* env = JVM::getInstance()->getEnv();
	jobject activityObj = JVM::getInstance()->getUnityActivity();
	jclass activityClass = JVM::getInstance()->getUnityActivityClass();

	jmethodID getFilesDirMethodId = env->GetMethodID(activityClass, "getCacheDir", "()Ljava/io/File;");
	jobject fileObj = env->CallObjectMethod(activityObj, getFilesDirMethodId);
	jclass fileClass = env->GetObjectClass(fileObj);

	jmethodID getAbsolutePathMethod = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
	jobject pathObj = env->CallObjectMethod(fileObj, getAbsolutePathMethod);
	_cachePath = string(env->GetStringUTFChars((jstring)pathObj, 0));
	return _cachePath;
}


std::string FileUtilsAndroid::getAppPath()
{
	return getApkPath();
}

std::string FileUtilsAndroid::getExecuteFilePath()
{
	return "class.dex";
}

std::string FileUtilsAndroid::getApkPath()
{
	if (!_apkPath.empty()) return _apkPath;

	JNIEnv* env = JVM::getInstance()->getEnv();
	jobject activityObj = JVM::getInstance()->getUnityActivity();
	jclass activityClass = JVM::getInstance()->getUnityActivityClass();

	jmethodID getPackageCodePathMethodId = env->GetMethodID(activityClass, "getPackageCodePath", "()Ljava/lang/String;");
	jobject apkPath = env->CallObjectMethod(activityObj, getPackageCodePathMethodId);
	_apkPath = string(env->GetStringUTFChars((jstring)apkPath, 0));

	return _apkPath;
}



zip* FileUtilsAndroid::openZipFile(const char* fileName)
{
	struct zip* apkFile = zip_open(fileName, 0, NULL);
	return apkFile;
}


void tiny::FileUtilsAndroid::closeZipFile(zip* zipFile)
{
	zip_close(zipFile);
}



NS_ENGINE_END

