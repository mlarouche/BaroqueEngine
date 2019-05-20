#pragma once

#include <cstdint>
#include <limits>

#include "ArchitectureDefines.h"
#include "CommonDefines.h"
#include "CompilerDefines.h"
#include "PlatformDefines.h"

#if defined(BAROQUE_DLL)
#if defined(BAROQUE_CORE_BULDING_DLL)
#define BAROQUE_CORE_API BAROQUE_DLL_EXPORT
#else
#define BAROQUE_CORE_API BAROQUE_DLL_IMPORT
#endif
#else
#define BAROQUE_CORE_API
#endif