#pragma once

#include "Core/CoreDefines.h"

#include "Core/Memory/FallbackAllocator.h"
#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/StackAllocator.h"

#if defined(BAROQUE_TRACE_MEMORY)
#include "Core/Memory/TracingAllocator.h"
#define BAROQUE_DEFINE_ALLOCATOR(Name, ...) using Name = Baroque::Memory::TracingAllocator<__VA_ARGS__>
#define BAROQUE_ALLOC(allocator, size, category) allocator.Allocate(size, BAROQUE_GET_MEMORY_CATEGORY(category), BAROQUE_SOURCE_LOCATION)
#else
#Define BAROQUE_DEFINE_ALLOCATOR(Name, ...) using Name = ___VA_ARGS__
#define BAROQUE_ALLOC(allocator, size, category) allocator.Allocate(size)
#endif

namespace Baroque
{
	namespace Memory
	{
		BAROQUE_DEFINE_ALLOCATOR(DefaultAllocator, Baroque::Memory::MallocAllocator);

		template<std::size_t Size>
		BAROQUE_DEFINE_ALLOCATOR(SmallAllocator, Baroque::Memory::FallbackAllocator<Baroque::Memory::StackAllocator<Size>, Baroque::Memory::MallocAllocator>);

		template<std::size_t Size>
		BAROQUE_DEFINE_ALLOCATOR(DefaultStackAllocator, Baroque::Memory::StackAllocator<Size>);
	}
}