LOCAL_ARM_MODE := arm

MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(MY_LOCAL_PATH)
PROJECT_PATH := $(LOCAL_PATH)/../../
INCLUDE_PATH := $(LOCAL_PATH)/../../luajit/src

include $(PROJECT_PATH)/libs/openssl/lib/android/opencrypto/Android.mk
include $(PROJECT_PATH)/support/libzip/Android.mk

include $(CLEAR_VARS)
 
LOCAL_MODULE    := TinyEngine

LOCAL_SRC_FILES :=\
	$(PROJECT_PATH)/TinyEngine.cpp \
	$(PROJECT_PATH)/platform/FileUtils.cpp \
	$(PROJECT_PATH)/platform/android/Common.cpp \
	$(PROJECT_PATH)/platform/android/FileUtilsAndroid.cpp \
	$(PROJECT_PATH)/platform/android/JVM.cpp \


LOCAL_C_INCLUDES :=\
	$(PROJECT_PATH) \
	$(PROJECT_PATH)/platform/android \

	
LOCAL_STATIC_LIBRARIES += opencrypto_static
LOCAL_STATIC_LIBRARIES += libzip


LOCAL_CPPFLAGS += -fexceptions

#LOCAL_LDFLAGS += -Wl,--version-script=$(MY_LOCAL_PATH)/export.map


LOCAL_LDLIBS+= -llog \
	-lz
include $(BUILD_SHARED_LIBRARY)