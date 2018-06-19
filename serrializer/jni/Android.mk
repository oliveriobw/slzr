#
# Android.mk
#

LOCAL_PATH   := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := slz-test


FILE_LIST := ../main.cpp
FILE_LIST += ../sink.cpp
FILE_LIST += ../serial_types_common.cpp
FILE_LIST += ../serial_types_map.cpp
FILE_LIST += ../base64.cpp



LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)

# uses symb links
LOCAL_C_INCLUDES += ..

LOCAL_CFLAGS   := -frtti -fsigned-char
LOCAL_CPPFLAGS := -frtti
LOCAL_CPP_FEATURES := exceptions rtti

CXXFLAGS=-g -std=c++11 -Wall -pedantic -Wc++11-extensions

# for android log method
LOCAL_LDLIBS := -llog -latomic -L$(SYSROOT)/usr/lib

include $(BUILD_SHARED_LIBRARY)

# Don't strip debug builds
ifeq ($(APP_OPTIM),debug)
  cmd-strip :=
endif

#$(info APP_OPTIM:$(APP_OPTIM))
#$(info local path:$(LOCAL_PATH))
#$(info source files:$(LOCAL_SRC_FILES) )
#$(info includes:$(LOCAL_C_INCLUDES) )
