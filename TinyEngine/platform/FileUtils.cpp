
#include "platform/FileUtils.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <iomanip>
#include <stdlib.h>
#include <openssl/md5.h>


//using namespace std;

NS_ENGINE_BEGIN

FileUtils* FileUtils::s_sharedFileUtils = NULL;

void FileUtils::purgeFileUtils()
{
	SAFE_DELETE(s_sharedFileUtils);
}


std::string FileUtils::getFileMD5(const char* pszFileName)
{
	// get file data
	unsigned long size = 0;
	const char* pData = getFileData(pszFileName, "r", &size);
	if (!pData)
	{
		string str("");
		return str;
	}

	// calculate md5
	MD5_CTX md5Ctx;
	unsigned char md5[17] = { 0 };
	MD5_Init(&md5Ctx);
	MD5_Update(&md5Ctx, pData, size);
	MD5_Final(md5, &md5Ctx);

	std::ostringstream outStr;
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		outStr << hex << setw(2) << setfill('0') << (int)md5[i];
	}

	// delete data
	delete[] pData;

	return outStr.str();
}




NS_ENGINE_END
