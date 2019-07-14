#pragma once

#include "Core/Memory/Memory.h"

class VerifyDeallocImpl : public Baroque::Memory::MallocAllocator
{
public:
	void* Allocate(const std::size_t size)
	{
		return MallocAllocator::Allocate(size);
	}

	void Deallocate(void* ptr)
	{
		LastDeallocation = ptr;
		++DeallocationCount;
		MallocAllocator::Deallocate(ptr);
	}

	static void Reset()
	{
		LastDeallocation = nullptr;
		DeallocationCount = 0;
	}

	static void* LastDeallocation;
	static std::size_t DeallocationCount;
};

BAROQUE_DEFINE_ALLOCATOR(VerifyDealloc, VerifyDeallocImpl);