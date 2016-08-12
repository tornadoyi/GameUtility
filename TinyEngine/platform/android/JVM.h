#ifndef __ENGINE_JVM_H__
#define __ENGINE_JVM_H__

#include <jni.h>

class JVM
{
public:
	static JVM* getInstance();

	JNIEnv* getEnv();

	jobject getUnityActivity();

	jclass getUnityActivityClass();

	jmethodID getUnityActivityMethodID(const char* name, const char* sig);

	static JVM* _instance;
};


#endif
