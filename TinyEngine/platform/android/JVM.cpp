#include <stdlib.h>
#include <android/log.h>
#include "JVM.h"

extern "C"
{
	JavaVM*		g_jvm;
	jobject		JavaClass;

	jint JNI_OnLoad(JavaVM* vm, void* reserved)
	{
		__android_log_print(ANDROID_LOG_INFO, "Unity", "[%s] Creating java vm\n", __FUNCTION__);
		g_jvm = vm;
		return JNI_VERSION_1_6;
	}

	void JNI_OnUnload(JavaVM* vm, void* reserved)
	{
		__android_log_print(ANDROID_LOG_INFO, "Unity", "[%s] \n", __FUNCTION__);
		g_jvm = NULL;
	}
}


JVM* JVM::_instance = NULL;

JVM* JVM::getInstance()
{
	if (_instance == NULL)
	{
		_instance = new JVM();
	}
	return _instance;
}

JNIEnv* JVM::getEnv()
{
	JNIEnv* jni_env = 0;
	g_jvm->AttachCurrentThread(&jni_env, 0);
	return jni_env;
}

jobject JVM::getUnityActivity()
{
	JNIEnv* env = getEnv();
	jclass unityClass = env->FindClass("com/unity3d/player/UnityPlayer");
	jfieldID activityFieldID = env->GetStaticFieldID(unityClass, "currentActivity", "Landroid/app/Activity;");
	jobject activityObj = env->GetStaticObjectField(unityClass, activityFieldID);
	return activityObj;
}

jclass JVM::getUnityActivityClass()
{
	JNIEnv* env = getEnv();
	jobject activityObj = getUnityActivity();
	jclass activityClass = env->GetObjectClass(activityObj);
	return activityClass;
}

jmethodID JVM::getUnityActivityMethodID(const char* name, const char* sig)
{
	JNIEnv* env = getEnv();
	jclass activityClass = getUnityActivityClass();
	return env->GetMethodID(activityClass, name, sig);
}
