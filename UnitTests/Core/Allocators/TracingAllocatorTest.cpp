#if defined(BAROQUE_TRACE_MEMORY)
#include <gtest/gtest.h>

#include "Core/Allocators/TracingAllocator.h"
#include "Core/Allocators/MallocAllocator.h"

namespace
{
	using TheTracingAllocator = Baroque::TracingAllocator<Baroque::MallocAllocator>;

	Baroque::TraceMemoryCategory Debug_Category_UnitTests("UnitTests");
	Baroque::TraceMemoryCategory Debug_Category_Test("CategoryTests");
}

TEST(TracingAllocator, ShouldRegisterTheAllocation)
{
	TheTracingAllocator allocator;
	void* result = allocator.Allocate(128, Debug_Category_UnitTests, BAROQUE_SOURCE_LOCATION);

	auto allocationInfo = Baroque::GetAllocationInfo(result);

	EXPECT_EQ(allocationInfo->Allocation, result);
	EXPECT_EQ(allocationInfo->Size, 128u);
	EXPECT_EQ(allocationInfo->Category, &Debug_Category_UnitTests);

	allocator.Deallocate(result);
}

TEST(TracingAllocator, ShouldUnregisterTheAllocation)
{
	TheTracingAllocator allocator;
	void* result = allocator.Allocate(128, Debug_Category_UnitTests, BAROQUE_SOURCE_LOCATION);

	EXPECT_TRUE(Baroque::GetAllocationInfo(result) != nullptr);

	allocator.Deallocate(result);

	EXPECT_TRUE(Baroque::GetAllocationInfo(result) == nullptr);
}

TEST(TracingAllocator, ShouldChangeTraceCategoryInfo)
{
	TheTracingAllocator allocator;
	void* result = allocator.Allocate(128, Debug_Category_Test, BAROQUE_SOURCE_LOCATION);

	auto categoryInfo = Baroque::GetTraceMemoryCategoryInfo(Debug_Category_Test);

	EXPECT_EQ(categoryInfo->AllocationCount, 1);
	EXPECT_EQ(categoryInfo->DeallocationCount, 0);
	EXPECT_EQ(categoryInfo->Category, &Debug_Category_Test);

	allocator.Deallocate(result);

	categoryInfo = Baroque::GetTraceMemoryCategoryInfo(Debug_Category_Test);

	EXPECT_EQ(categoryInfo->DeallocationCount, 1);
}
#endif