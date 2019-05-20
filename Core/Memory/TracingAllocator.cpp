#include "TracingAllocator.h"

#if defined(BAROQUE_TRACE_MEMORY)
#include "Core/Memory/PoolAllocator.h"
#include "Core/Memory/VirtualMemoryAllocator.h"
#include "Core/Hashing/PointerHash.h"
#include "Core/Threading/ReaderWriterLock.h"
#include "Core/Memory/MemorySize.h"

namespace Baroque
{
	namespace Memory
	{
		using namespace Literals;

		constexpr std::size_t AllocationInfoTableSize = 16 * 1024;
		constexpr std::size_t TraceMemoryCategoryTableSize = 128;

		constexpr std::size_t PageSize = 64_KB;

		static_assert((AllocationInfoTableSize& (AllocationInfoTableSize - 1)) == 0, "The Allocation Info Table must be a power of two");
		static_assert((TraceMemoryCategoryTableSize& (TraceMemoryCategoryTableSize - 1)) == 0, "The Debug Memory Category Info Table must be a power of two");

		struct AllocationInfoNode : public AllocationInfo
		{
			AllocationInfoNode* Next;
		};

		struct TraceMemoryCategoryNode : public TraceMemoryCategoryInfo
		{
			TraceMemoryCategoryNode* Next;
		};

		using AllocationInfoNodeAllocatorType = PoolObjectAllocator<AllocationInfoNode, VirtualMemoryAllocator, (PageSize - sizeof(void*)) / sizeof(AllocationInfoNode)>;
		using TraceCategoryNodeAllocatorType = PoolObjectAllocator<TraceMemoryCategoryNode, VirtualMemoryAllocator, (PageSize - sizeof(void(*))) / sizeof(TraceMemoryCategoryNode)>;

		AllocationInfoNodeAllocatorType AllocationInfoNodeAllocator;
		TraceCategoryNodeAllocatorType TraceCategoryNodeAllocator;

		AllocationInfoNode* AllocationInfoTable[AllocationInfoTableSize];
		TraceMemoryCategoryNode* TraceCategoryInfoTable[TraceMemoryCategoryTableSize];

		ReaderWriterLock AllocLock;

		void RegisterOrUpdateCategory(const TraceMemoryCategory* category)
		{
			auto categoryHashIndex = Hashing::PointerHash(category) & (TraceMemoryCategoryTableSize - 1);

			auto * *categoryIt = &TraceCategoryInfoTable[categoryHashIndex];

			while (categoryIt && *categoryIt)
			{
				if ((*categoryIt)->Category == category)
				{
					break;
				}

				categoryIt = &(*categoryIt)->Next;
			}

			if (*categoryIt)
			{
				++(*categoryIt)->AllocationCount;
			}
			else
			{
				*categoryIt = TraceCategoryNodeAllocator.Allocate();
				(*categoryIt)->Category = category;
				++(*categoryIt)->AllocationCount;
			}
		}

		void UpdateCategoryOnUnregisterAllocation(const TraceMemoryCategory* category)
		{
			auto categoryHashIndex = Hashing::PointerHash(category) & (TraceMemoryCategoryTableSize - 1);

			auto * categoryIt = TraceCategoryInfoTable[categoryHashIndex];

			while (categoryIt)
			{
				if (categoryIt->Category == category)
				{
					++categoryIt->DeallocationCount;
					break;
				}

				categoryIt = categoryIt->Next;
			}
		}

		void RegisterAllocation(const void* allocation, const std::size_t size, const TraceMemoryCategory& category, const Baroque::SourceLocation& sourceLocation)
		{
			if (!allocation)
			{
				return;
			}

			Baroque::AutoWriteLock autoLock(AllocLock);

			AllocationInfoNode* allocInfo = AllocationInfoNodeAllocator.Allocate();

			allocInfo->Allocation = allocation;
			allocInfo->Category = &category;
			allocInfo->Size = size;
			allocInfo->SourceLocation = sourceLocation;

			auto allocHash = Hashing::PointerHash(allocation) & (AllocationInfoTableSize - 1);
			allocInfo->Next = AllocationInfoTable[allocHash];
			AllocationInfoTable[allocHash] = allocInfo;

			RegisterOrUpdateCategory(&category);
		}

		void UnregisterAllocation(const void* allocation)
		{
			if (!allocation)
			{
				return;
			}

			Baroque::AutoWriteLock autoLock(AllocLock);

			auto allocHash = Hashing::PointerHash(allocation) & (AllocationInfoTableSize - 1);

			auto * *allocIt = &AllocationInfoTable[allocHash];
			auto * *previous = &AllocationInfoTable[allocHash];

			while (allocIt)
			{
				if ((*allocIt)->Allocation == allocation)
				{
					UpdateCategoryOnUnregisterAllocation((*allocIt)->Category);

					(*previous)->Next = (*allocIt)->Next;
					AllocationInfoNodeAllocator.Deallocate(*allocIt);
					*allocIt = nullptr;
					break;
				}

				previous = allocIt;
				allocIt = &(*allocIt)->Next;
			}
		}

		const AllocationInfo* GetAllocationInfo(const void* allocation)
		{
			if (!allocation)
			{
				return nullptr;
			}

			Baroque::AutoReadLock autoLock(AllocLock);

			auto allocHash = Hashing::PointerHash(allocation) & (AllocationInfoTableSize - 1);

			auto * allocIt = AllocationInfoTable[allocHash];

			while (allocIt)
			{
				if (allocIt->Allocation == allocation)
				{
					return allocIt;
				}

				allocIt = allocIt->Next;
			}

			return nullptr;
		}

		const TraceMemoryCategoryInfo* GetTraceMemoryCategoryInfo(const TraceMemoryCategory & category)
		{
			return GetTraceMemoryCategoryInfo(&category);
		}

		const TraceMemoryCategoryInfo* GetTraceMemoryCategoryInfo(const TraceMemoryCategory * category)
		{
			Baroque::AutoReadLock autoLock(AllocLock);

			auto categoryHashIndex = Hashing::PointerHash(category) & (TraceMemoryCategoryTableSize - 1);

			auto * categoryIt = TraceCategoryInfoTable[categoryHashIndex];

			while (categoryIt)
			{
				if (categoryIt->Category == category)
				{
					return categoryIt;
				}

				categoryIt = categoryIt->Next;
			}

			return nullptr;
		}
	}
}
#endif