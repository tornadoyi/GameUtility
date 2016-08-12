
#include "platform/Common.h"
#include <android/log.h>
#include <stdio.h>
#include <jni.h>

NS_ENGINE_BEGIN

#define MAX_LEN         (tiny::kMaxLogLen + 1)

void EngineLog(const char * pszFormat, ...)
{
    char buf[MAX_LEN];

    va_list args;
    va_start(args, pszFormat);
    vsnprintf(buf, MAX_LEN, pszFormat, args);
    va_end(args);

    __android_log_print(ANDROID_LOG_INFO, "Unity", "%s", buf);
}



NS_ENGINE_END
