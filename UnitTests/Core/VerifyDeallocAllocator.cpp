#include "VerifyDeallocAllocator.h"

void* VerifyDeallocImpl::LastDeallocation = nullptr;
std::size_t VerifyDeallocImpl::DeallocationCount = 0;