#include <gtest/gtest.h>

#include "Core/Memory/SegregatorAllocator.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/StackAllocator.h"

namespace
{
	using TheSegregatorAllocator = Baroque::Memory::SegregatorAllocator<1024, Baroque::Memory::StackAllocator<1024>, Baroque::Memory::MallocAllocator>;
}

TEST(SegregatorAllocator, ShouldAllocateWithSmallAllocator)
{
	TheSegregatorAllocator allocator;

	void* result = allocator.Allocate(128);

	EXPECT_TRUE(result != nullptr);
	EXPECT_TRUE(result >= &allocator && result < (&allocator + 1024));

	allocator.Deallocate(result);
}

TEST(SegregatorAllocator, ShouldAllocateWithLargeAllocator)
{
	TheSegregatorAllocator allocator;

	void* result = allocator.Allocate(2048);

	EXPECT_TRUE(result != nullptr);
	EXPECT_FALSE(result >= &allocator && result < (&allocator + 1024));

	allocator.Deallocate(result);
}