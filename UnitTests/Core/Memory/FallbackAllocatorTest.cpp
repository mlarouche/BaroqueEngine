#include <gtest/gtest.h>

#include "Core/Memory/FallbackAllocator.h"
#include "Core/Memory/StackAllocator.h"
#include "Core/Memory/MallocAllocator.h"

namespace
{
	using TheFallbackAllocator = Baroque::Memory::FallbackAllocator<Baroque::Memory::StackAllocator<512>, Baroque::Memory::MallocAllocator>;
}

TEST(FallbackAllocator, ShouldAllocateUsingPrimaryAllocator)
{
	TheFallbackAllocator allocator;

	void* result = allocator.Allocate(256);

	EXPECT_TRUE(result != nullptr);
	EXPECT_TRUE(result >= &allocator && result <= (&allocator + 512));

	allocator.Deallocate(result);
}

TEST(FallbackAllocator, ShouldAllocateUsingFallbackAllocator)
{
	TheFallbackAllocator allocator;

	void* result = allocator.Allocate(1024);

	EXPECT_TRUE(result != nullptr);
	EXPECT_FALSE(result >= &allocator && result <= (&allocator + 512));

	allocator.Deallocate(result);
}

TEST(FallbackAllocator, ShouldUseFallbackWhenPrimaryIsFull)
{
	TheFallbackAllocator allocator;

	void* firstAlloc = allocator.Allocate(256);
	EXPECT_TRUE(firstAlloc != nullptr);
	EXPECT_TRUE(firstAlloc >= &allocator && firstAlloc <= (&allocator + 512));

	void* secondAlloc = allocator.Allocate(256);
	EXPECT_TRUE(secondAlloc != nullptr);
	EXPECT_TRUE(secondAlloc >= &allocator && secondAlloc <= (&allocator + 512));

	void* thirdAlloc = allocator.Allocate(256);
	EXPECT_TRUE(thirdAlloc != nullptr);
	EXPECT_FALSE(thirdAlloc >= &allocator && thirdAlloc <= (&allocator + 512));

	allocator.Deallocate(thirdAlloc);
	allocator.Deallocate(secondAlloc);
	allocator.Deallocate(firstAlloc);
}

TEST(FallbackAllocator, ShouldFailWhenBothAllocatorsAreFull)
{
	Baroque::Memory::FallbackAllocator<Baroque::Memory::StackAllocator<128>, Baroque::Memory::StackAllocator<256>> allocator;

	EXPECT_TRUE(allocator.Allocate(1024) == nullptr);
}