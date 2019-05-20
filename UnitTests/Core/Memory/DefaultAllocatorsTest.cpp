#include <gtest/gtest.h>

#include "Core/Memory/Memory.h"

namespace
{
	BAROQUE_REGISTER_MEMORY_CATEGORY(TestDefaultAllocator)
}

TEST(DefaultAllocator, TestDefaultAllocator)
{
	Baroque::Memory::DefaultAllocator allocator;

	auto testMemory = BAROQUE_ALLOC(allocator, 256, TestDefaultAllocator);

	EXPECT_TRUE(testMemory != nullptr);

	allocator.Deallocate(testMemory);
}

TEST(DefaultAllocator, TestSmallAllocator)
{
	Baroque::Memory::SmallAllocator<256> allocator;

	auto testMemory = BAROQUE_ALLOC(allocator, 32, TestDefaultAllocator);

	EXPECT_TRUE(testMemory != nullptr);

	allocator.Deallocate(testMemory);
}

TEST(DefaultAllocator, TestDefaultStackAllocator)
{
	Baroque::Memory::DefaultStackAllocator<256> allocator;

	auto testMemory = BAROQUE_ALLOC(allocator, 32, TestDefaultAllocator);

	EXPECT_TRUE(testMemory != nullptr);

	allocator.Deallocate(testMemory);
}