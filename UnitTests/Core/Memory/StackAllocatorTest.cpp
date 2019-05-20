#include <gtest/gtest.h>

#include "Core/Memory/StackAllocator.h"

TEST(StackAllocator, ShouldProperlyAllocate)
{
	Baroque::Memory::StackAllocator<256> allocator;

	EXPECT_TRUE(allocator.Allocate(128) != nullptr);
}

TEST(StackAllocator, ShouldFailOnOversizeAllocation)
{
	Baroque::Memory::StackAllocator<256> allocator;

	EXPECT_TRUE(allocator.Allocate(1024) == nullptr);
}

TEST(StackAllocator, ShouldFailWhenExceedingSize)
{
	Baroque::Memory::StackAllocator<128> allocator;

	EXPECT_TRUE(allocator.Allocate(64) != nullptr);
	EXPECT_TRUE(allocator.Allocate(64) != nullptr);
	EXPECT_TRUE(allocator.Allocate(64) == nullptr);
}