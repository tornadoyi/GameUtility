
#include "platform/Common.h"
#include <stdio.h>

#import <UIKit/UIAlert.h>


NS_ENGINE_BEGIN

#define MAX_LEN         (tiny::kMaxLogLen + 1)

void EngineLog(const char * pszFormat, ...)
{
    printf("Unity: ");
    char szBuf[kMaxLogLen+1] = {0};
    va_list ap;
    va_start(ap, pszFormat);
    vsnprintf(szBuf, kMaxLogLen, pszFormat, ap);
    va_end(ap);
    printf("%s", szBuf);
    printf("\n");
}



NS_ENGINE_END
