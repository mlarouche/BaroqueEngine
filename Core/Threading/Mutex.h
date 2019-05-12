#pragma once

#include "Core/CoreDefines.h"

#include "Core/Threading/AutoLock.h"

#if defined(BAROQUE_PLATFORM_WINDOWS)
#include "Core/Platforms/Win32/Threading/Mutex.h"
#else
#error "Implement Mutex for your platform"
#endif
