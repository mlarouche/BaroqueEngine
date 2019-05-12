#pragma once

#if defined(_MSC_VER)
#define BAROQUE_COMPILER_MSVC 1
#elif defined(__clang__)
#define BAROQUE_COMPILER_CLANG 1
#elif defined(__GNUC__)
#define BAROQUE_COMPILER_GCC 1
#elif defined(__INTEL_COMPILER)
#define BAROQUE_COMPILER_INTEL 1
#endif