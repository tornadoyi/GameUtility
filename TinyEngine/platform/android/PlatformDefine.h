#ifndef __ENGINE_PLATFORMDEFINE_H__
#define __ENGINE_PLATFORMDEFINE_H__

#include "android/log.h"

#define ENGINE_DLL

#define CC_NO_MESSAGE_PSEUDOASSERT(cond)                        \
    if (!(cond)) {                                              \
        __android_log_print(ANDROID_LOG_ERROR,                  \
                            "Unity",                 \
                            "%s function:%s line:%d",           \
                            __FILE__, __FUNCTION__, __LINE__);  \
    }

#define CC_MESSAGE_PSEUDOASSERT(cond, msg)                          \
    if (!(cond)) {                                                  \
        __android_log_print(ANDROID_LOG_ERROR,                      \
                            "Unity",                     \
                            "file:%s function:%s line:%d, %s",      \
                            __FILE__, __FUNCTION__, __LINE__, msg); \
    }

#define ENGINE_ASSERT(cond) CC_NO_MESSAGE_PSEUDOASSERT(cond)

#define ENGINE_UNUSED_PARAM(unusedparam) (void)unusedparam

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#endif /* __ENGINE_PLATFORMDEFINE_H__*/
