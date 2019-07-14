#pragma once

#include "Core/CoreDefines.h"

#if defined(BAROQUE_TRACE_MEMORY)
#include "Core/Utilities/SourceLocation.h"

namespace Baroque
{
	namespace Memory
	{
		struct BAROQUE_CORE_API TraceMemoryCategory
		{
			constexpr TraceMemoryCategory(const char* name)
				: Name(name)
			{
			}

			const char* Name;
		};

		struct BAROQUE_CORE_API AllocationInfo
		{
			const void* Allocation;
			std::size_t Size;
			Baroque::SourceLocation SourceLocation;
			const TraceMemoryCategory* Category;
		};

		struct BAROQUE_CORE_API TraceMemoryCategoryInfo
		{
			const TraceMemoryCategory* Category;
			std::size_t AllocationCount;
			std::size_t DeallocationCount;
		};

		BAROQUE_CORE_API void RegisterAllocation(const void* allocation, const std::size_t size, const TraceMemoryCategory& category, const Baroque::SourceLocation& sourceLocation);
		BAROQUE_CORE_API void UnregisterAllocation(const void* allocation);
		BAROQUE_CORE_API const AllocationInfo* GetAllocationInfo(const void* allocation);
		BAROQUE_CORE_API const TraceMemoryCategoryInfo* GetTraceMemoryCategoryInfo(const TraceMemoryCategory& category);
		BAROQUE_CORE_API const TraceMemoryCategoryInfo* GetTraceMemoryCategoryInfo(const TraceMemoryCategory* category);

		template<typename Allocator>
		class TracingAllocator : private Allocator
		{
		public:
			static constexpr std::size_t StackCapacity = Allocator::StackCapacity;

			void* Allocate(const std::size_t size, const TraceMemoryCategory& category, const Baroque::SourceLocation& sourceLocation)
			{
				void* alloc = Allocator::Allocate(size);
				if (alloc)
				{
					RegisterAllocation(alloc, size, category, sourceLocation);
				}

				return alloc;
			}

			void Deallocate(void* ptr)
			{
				UnregisterAllocation(ptr);

				Allocator::Deallocate(ptr);
			}

			bool Owns(const void* ptr) const
			{
				return Allocator::Owns(ptr);
			}
		};
	}
}
#endif

#if defined(BAROQUE_TRACE_MEMORY)
#define BAROQUE_EXTERN_MEMORY_CATEGORY(Name) extern Baroque::Memory::TraceMemoryCategory _Baroque_Trace_Category_##Name;
#define BAROQUE_REGISTER_MEMORY_CATEGORY(Name) Baroque::Memory::TraceMemoryCategory _Baroque_Trace_Category_##Name(#Name);
#define BAROQUE_GET_MEMORY_CATEGORY(Name) _Baroque_Trace_Category_##Name
#else
#define BAROQUE_EXTERN_MEMORY_CATEGORY(Name)
#define BAROQUE_REGISTER_MEMORY_CATEGORY(Name)
#define BAROQUE_GET_MEMORY_CATEGORY(Name)
#endif
