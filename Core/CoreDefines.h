#pragma once

#include <cstdint>
#include <limits>

#include "ArchitectureDefines.h"
#include "CompilerDefines.h"
#include "PlatformDefines.h"

// Need to be included after because it could use define from previous includes
#include "CommonDefines.h"

#if defined(BAROQUE_DLL)
#if defined(BAROQUE_CORE_BULDING_DLL)
#define BAROQUE_CORE_API BAROQUE_DLL_EXPORT
#else
#define BAROQUE_CORE_API BAROQUE_DLL_IMPORT
#endif
#else
#define BAROQUE_CORE_API
#endif