#include "platform/Common.h"
#include "platform/FileUtils.h"

#include <string>

using namespace tiny;

int string2str(string str, unsigned char** p)
{
	int length = str.length();
	if (p == NULL || length == 0) return 0;
	*p = ALLOC(length);
	memcpy(*p, str.c_str(), length);
	return length;
}

extern "C"
{
	// memory
	void memory_free(void* ptr) { SAFE_FREE(ptr); }


	// file
	bool file_exist(const char* fileName) { return FileUtils::sharedFileUtils()->isFileExist(fileName); }

	int file_data_path(unsigned char** path) { return string2str(FileUtils::sharedFileUtils()->getDataPath(), path); }

    int file_persistent_data_path(unsigned char** path){ return string2str(FileUtils::sharedFileUtils()->getPersistentDataPath(), path); }
    
	int file_temporary_cache_path(unsigned char** path){ return string2str(FileUtils::sharedFileUtils()->getTemporaryCachePath(), path); }

	int file_app_path(unsigned char** path) { return string2str(FileUtils::sharedFileUtils()->getAppPath(), path); }

	int file_exe_path(unsigned char** path) { return string2str(FileUtils::sharedFileUtils()->getExecuteFilePath(), path); }

	int file_md5(const char* fileName, unsigned char** md5) { return string2str(FileUtils::sharedFileUtils()->getFileMD5(fileName), md5); }

}