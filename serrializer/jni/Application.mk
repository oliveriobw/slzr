# File: Application.mk

#APP_PROJECT_PATH =
#APP_CPPFLAGS += -Isources/bar
#APP_LDFLAGS
#APP_STL
#APP_SHORT_COMMANDS
#NDK_TOOLCHAIN_VERSION
#APP_PIE
#APP_THIN_ARCHIVE
#APP_BUILD_SCRIPT
#APP_ABI := armeabi armeabi-v7a x86 mips
#APP_PLATFORM

# APP_STL options:
# system stlport_static stlport_shared gnustl_static gnustl_shared c++_static c++_shared none

#APP_ABI := armeabi 
APP_ABI := x86 

# debug
APP_OPTIM = debug


# The only one with C++11 support
APP_STL := c++_static

$(info "APP_CFLAGS==>" $(APP_CFLAGS) )
$(info "APP_STL==>" $(APP_STL) )
$(info "APP_OPTIM==>" $(APP_OPTIM) )
