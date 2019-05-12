#pragma once

#if defined(__amd64__) || defined(__x86_64__) || defined(_M_AMD64)
#define BAROQUE_ARCHITECTURE_X64 1
#elif defined(__arm__) || defined(_M_ARM)
#define BAROQUE_ARCHITECTURE_ARM 1
#elif defined(__thumb__) || defined(_M_ARMT)
#define BAROQUE_ARCHITECTURE_ARM_THUMB 1
#elif defined(__aarch64__)
#define BAROQUE_ARCHITECTURE_AARCH64 1
#endif