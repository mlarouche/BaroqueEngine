#if defined(BAROQUE_TRACE_MEMORY)
#include <gtest/gtest.h>

#include "Core/Memory/TracingAllocator.h"
#include "Core/Memory/MallocAllocator.h"

namespace
{
	using TheTracingAllocator = Baroque::Memory::TracingAllocator<Baroque::Memory::MallocAllocator>;

	Baroque::Memory::TraceMemoryCategory Debug_Category_UnitTests("UnitTests");
	Baroque::Memory::TraceMemoryCategory CategoryTest("CategoryTests");
}

TEST(TracingAllocator, ShouldRegisterTheAllocation)
{
	TheTracingAllocator allocator;
	void* result = allocator.Allocate(128, Debug_Category_UnitTests, BAROQUE_SOURCE_LOCATION);

	auto allocationInfo = Baroque::Memory::GetAllocationInfo(result);

	EXPECT_EQ(allocationInfo->Allocation, result);
	EXPECT_EQ(allocationInfo->Size, 128u);
	EXPECT_EQ(allocationInfo->Category, &Debug_Category_UnitTests);

	allocator.Deallocate(result);
}

TEST(TracingAllocator, ShouldUnregisterTheAllocation)
{
	TheTracingAllocator allocator;
	void* result = allocator.Allocate(128, Debug_Category_UnitTests, BAROQUE_SOURCE_LOCATION);

	EXPECT_TRUE(Baroque::Memory::GetAllocationInfo(result) != nullptr);

	allocator.Deallocate(result);

	EXPECT_TRUE(Baroque::Memory::GetAllocationInfo(result) == nullptr);
}

TEST(TracingAllocator, ShouldChangeTraceCategoryInfo)
{
	TheTracingAllocator allocator;
	void* result = allocator.Allocate(128, CategoryTest, BAROQUE_SOURCE_LOCATION);

	auto categoryInfo = Baroque::Memory::GetTraceMemoryCategoryInfo(CategoryTest);

	EXPECT_EQ(categoryInfo->AllocationCount, 1);
	EXPECT_EQ(categoryInfo->DeallocationCount, 0);
	EXPECT_EQ(categoryInfo->Category, &CategoryTest);

	allocator.Deallocate(result);

	categoryInfo = Baroque::Memory::GetTraceMemoryCategoryInfo(CategoryTest);

	EXPECT_EQ(categoryInfo->DeallocationCount, 1);
}
#endif