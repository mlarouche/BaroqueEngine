#include <gtest/gtest.h>

#include "Core/Memory/PoolAllocator.h"
#include "Core/Memory/MallocAllocator.h"

namespace
{
	struct TestAllocator
	{
		static constexpr std::size_t StackCapacity = 0;

		void* Allocate(const std::size_t size)
		{
			++AllocateCount;
			return Mallocator.Allocate(size);
		}

		void Deallocate(void* ptr)
		{
			++DellocateCount;
			Mallocator.Deallocate(ptr);
		}

		Baroque::Memory::MallocAllocator Mallocator;

		static std::size_t AllocateCount;
		static std::size_t DellocateCount;
	};

	std::size_t TestAllocator::AllocateCount = 0;
	std::size_t TestAllocator::DellocateCount = 0;

	using TestPoolAllocator = Baroque::Memory::PoolAllocator<TestAllocator, 64, 32>;
}

TEST(PoolAllocator, ShouldProperlyDeallocateMemoryBlock)
{
	TestAllocator::AllocateCount = 0;
	TestAllocator::DellocateCount = 0;

	{
		TestPoolAllocator allocator;

		for (std::size_t i = 0; i < 64; ++i)
		{
			allocator.Allocate();
		}

		EXPECT_EQ(TestAllocator::AllocateCount, 2);
	}

	EXPECT_EQ(TestAllocator::DellocateCount, 2);
}

TEST(PoolAllocator, ShouldReturnTheSamePointerWhenAllocatingAfterAFree)
{
	TestPoolAllocator allocator;

	void* first = allocator.Allocate();
	allocator.Deallocate(first);

	void* second = allocator.Allocate();

	EXPECT_EQ(first, second);

	allocator.Deallocate(second);
}