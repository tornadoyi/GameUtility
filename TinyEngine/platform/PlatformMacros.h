
#ifndef __ENGINE_PLATFORM_MACROS_H__
#define __ENGINE_PLATFORM_MACROS_H__


// generic macros

// namespace cocos2d {}
#ifdef __cplusplus
    #define NS_ENGINE_BEGIN                     namespace tiny {
    #define NS_ENGINE_END                       }
    #define USING_NS_ENGINE                     using namespace tiny
#else
    #define NS_BEGIN 
    #define NS_END 
    #define USING_NS_CC 
#endif 

/** PROPERTY_READONLY is used to declare a protected variable.
 We can use getter to read the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 @warning : The getter is a public virtual function, you should rewrite it first.
 The variables and methods declared after PROPERTY_READONLY are all public.
 If you need protected or private, please declare.
 */
#define PROPERTY_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);

#define PROPERTY_READONLY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void);

/** PROPERTY is used to declare a protected variable.
 We can use getter to read the variable, and use the setter to change the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 "set + funName" is the name of the setter.
 @warning : The getter and setter are public virtual functions, you should rewrite them first.
 The variables and methods declared after PROPERTY are all public.
 If you need protected or private, please declare.
 */
#define PROPERTY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void);\
public: virtual void set##funName(varType var);

#define PROPERTY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void);\
public: virtual void set##funName(const varType& var);

/** SYNTHESIZE_READONLY is used to declare a protected variable.
 We can use getter to read the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 @warning : The getter is a public inline function.
 The variables and methods declared after SYNTHESIZE_READONLY are all public.
 If you need protected or private, please declare.
 */
#define SYNTHESIZE_READONLY(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }

#define SYNTHESIZE_READONLY_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }

/** SYNTHESIZE is used to declare a protected variable.
 We can use getter to read the variable, and use the setter to change the variable.
 @param varType : the type of variable.
 @param varName : variable name.
 @param funName : "get + funName" is the name of the getter.
 "set + funName" is the name of the setter.
 @warning : The getter and setter are public  inline functions.
 The variables and methods declared after SYNTHESIZE are all public.
 If you need protected or private, please declare.
 */
#define SYNTHESIZE(varType, varName, funName)\
protected: varType varName;\
public: virtual varType get##funName(void) const { return varName; }\
public: virtual void set##funName(varType var){ varName = var; }

#define SYNTHESIZE_PASS_BY_REF(varType, varName, funName)\
protected: varType varName;\
public: virtual const varType& get##funName(void) const { return varName; }\
public: virtual void set##funName(const varType& var){ varName = var; }

#define SYNTHESIZE_RETAIN(varType, varName, funName)    \
private: varType varName; \
public: virtual varType get##funName(void) const { return varName; } \
public: virtual void set##funName(varType var)   \
{ \
    if (varName != var) \
    { \
        SAFE_RETAIN(var); \
        SAFE_RELEASE(varName); \
        varName = var; \
    } \
} 

#define ALLOC(s) (unsigned char*)calloc(s, sizeof(unsigned char))

#define SAFE_DELETE(p)            do { if(p) { delete (p); (p) = 0; } } while(0)
#define SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = 0; } } while(0)
#define SAFE_FREE(p)                do { if(p) { free(p); (p) = 0; } } while(0)
#define SAFE_RELEASE(p)            do { if(p) { (p)->release(); } } while(0)
#define SAFE_RELEASE_NULL(p)        do { if(p) { (p)->release(); (p) = 0; } } while(0)
#define SAFE_RETAIN(p)            do { if(p) { (p)->retain(); } } while(0)
#define BREAK_IF(cond)            if(cond) break



// cocos2d debug
#define LOG(format, ...)      tiny::EngineLog(format, ##__VA_ARGS__)
#define LOGERROR(format,...)  tiny::EngineLog(format, ##__VA_ARGS__)
#define LOGINFO(format,...)   tiny::EngineLog(format, ##__VA_ARGS__)




/*
 * only certain compilers support __attribute__((deprecated))
 */
#if defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
    #define DEPRECATED_ATTRIBUTE __attribute__((deprecated))
#elif _MSC_VER >= 1400 //vs 2005 or higher
    #define DEPRECATED_ATTRIBUTE __declspec(deprecated) 
#else
    #define DEPRECATED_ATTRIBUTE
#endif 

/*
 * only certain compiler support __attribute__((format))
 * formatPos - 1-based position of format string argument
 * argPos - 1-based position of first format-dependent argument
 */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#elif defined(__has_attribute)
  #if __has_attribute(format)
  #define FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
  #endif // __has_attribute(format)
#else
#define FORMAT_PRINTF(formatPos, argPos)
#endif

#if defined(_MSC_VER)
#define FORMAT_PRINTF_SIZE_T "%08lX"
#else
#define FORMAT_PRINTF_SIZE_T "%08zX"
#endif

#ifdef __GNUC__
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif

#endif // __PLATFORM_MACROS_H__
