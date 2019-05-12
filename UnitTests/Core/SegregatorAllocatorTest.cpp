#include <gtest/gtest.h>

#include "Core/Allocators/SegregatorAllocator.h"
#include "Core/Allocators/MallocAllocator.h"
#include "Core/Allocators/StackAllocator.h"

TEST(SegregatorAllocator, ShouldAllocateWithSmallAllocator)
{
	Baroque::SegregatorAllocator<1024, Baroque::StackAllocator<1024>, Baroque::MallocAllocator> allocator;

	void* result = allocator.Allocate(128);

	EXPECT_TRUE(result != nullptr);
	EXPECT_TRUE(result >= &allocator && result < (&allocator + 1024));

	allocator.Deallocate(result);
}

TEST(SegregatorAllocator, ShouldAllocateWithLargeAllocator)
{
	Baroque::SegregatorAllocator<1024, Baroque::StackAllocator<1024>, Baroque::MallocAllocator> allocator;

	void* result = allocator.Allocate(2048);

	EXPECT_TRUE(result != nullptr);
	EXPECT_FALSE(result >= &allocator && result < (&allocator + 1024));

	allocator.Deallocate(result);
}