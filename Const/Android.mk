LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE     := const
LOCAL_SRC_FILES  := const.cpp

include $(BUILD_SHARED_LIBRARY)

