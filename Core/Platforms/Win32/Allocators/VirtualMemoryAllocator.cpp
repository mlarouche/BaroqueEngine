#include "Core/Memory/VirtualMemoryAllocator.h"

#include "Core/Platforms/Win32/MinimalWindowsIncludes.h"
#include <memoryapi.h>

namespace Baroque
{
	namespace Memory
	{
		void* VirtualMemoryAllocator::Allocate(const std::size_t size)
		{
			return ::VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);
		}

		void VirtualMemoryAllocator::Deallocate(void* ptr)
		{
			::VirtualFree(ptr, 0, MEM_RELEASE);
		}
	}
}