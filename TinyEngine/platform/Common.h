
#ifndef __ENGINE_COMMON_H__
#define __ENGINE_COMMON_H__

#include "PlatformDefine.h"
#include "platform/PlatformMacros.h"
#include "stdc.h"

NS_ENGINE_BEGIN

/**
 * @addtogroup platform
 * @{
 */

/// The max length of CCLog message.
static const int kMaxLogLen = 16*1024;

/**
@brief Output Debug message.
*/
void ENGINE_DLL EngineLog(const char * pszFormat, ...) FORMAT_PRINTF(1, 2);



// end of platform group
/// @}

NS_ENGINE_END

#endif    // __CC_COMMON_H__
