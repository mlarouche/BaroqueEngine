#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define BAROQUE_PLATFORM_WINDOWS 1
#elif defined(__linux__)
#define BAROQUE_PLATFORM_LINUX 1
#elif defined(__ANDROID__)
#define BAROQUE_PLATFORM_ANDROID 1
#elif defined(__APPLE__)
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR
#define BAROQUE_PLATFORM_IOS 1
#define BAROQUE_PLATFORM_IOS_SIMULATOR 1
#elif TARGET_OS_IPHONE
#define BAROQUE_PLATFORM_IOS 1
#elif TARGET_OS_MAC
#define BAROQUE_PLATFORM_MAC 1
#else
#error "Unknown Apple platform"
#endif
#endif
