#include <gtest/gtest.h>

#include "Core/Allocators/FallbackAllocator.h"
#include "Core/Allocators/StackAllocator.h"
#include "Core/Allocators/MallocAllocator.h"

TEST(FallbackAllocator, ShouldAllocateUsingPrimaryAllocator)
{
	Baroque::FallbackAllocator<Baroque::StackAllocator<512>, Baroque::MallocAllocator> allocator;

	void* result = allocator.Allocate(256);

	EXPECT_TRUE(result != nullptr);
	EXPECT_TRUE(result >= &allocator && result <= (&allocator + 512));

	allocator.Deallocate(result);
}

TEST(FallbackAllocator, ShouldAllocateUsingFallbackAllocator)
{
	Baroque::FallbackAllocator<Baroque::StackAllocator<512>, Baroque::MallocAllocator> allocator;

	void* result = allocator.Allocate(1024);

	EXPECT_TRUE(result != nullptr);
	EXPECT_FALSE(result >= &allocator && result <= (&allocator + 512));

	allocator.Deallocate(result);
}

TEST(FallbackAllocator, ShouldUseFallbackWhenPrimaryIsFull)
{
	Baroque::FallbackAllocator<Baroque::StackAllocator<512>, Baroque::MallocAllocator> allocator;

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
	Baroque::FallbackAllocator<Baroque::StackAllocator<128>, Baroque::StackAllocator<256>> allocator;

	EXPECT_TRUE(allocator.Allocate(1024) == nullptr);
}