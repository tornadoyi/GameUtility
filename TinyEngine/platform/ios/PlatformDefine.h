#ifndef __ENGINE_PLATFORMDEFINE_H__
#define __ENGINE_PLATFORMDEFINE_H__

#include <assert.h>

#define ENGINE_DLL 

#define ENGINE_ASSERT(cond) assert(cond)


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
