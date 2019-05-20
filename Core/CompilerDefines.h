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

#if defined(BAROQUE_COMPILER_MSVC)
#define BAROQUE_DLL_EXPORT __declspec(dllexport)
#define BAROQUE_DLL_IMPORT __declspec(dllimport)
#elif defined(BAROQUE_COMPILER_GCC) || defined(BAROQUE_COMPILER_CLANG)
#define BAROQUE_DLL_EXPORT __attribute__ ((visibility("default")))
#define BAROQUE_DLL_IMPORT __attribute__ ((visibility("default")))
#else
#define BAROQUE_DLL_EXPORT
#define BAROQUE_DLL_IMPORT
#endif