#include "MallocAllocator.h"

#include <cstdlib>

namespace Baroque
{
	namespace Memory
	{
		void* MallocAllocator::Allocate(const std::size_t size)
		{
			return std::malloc(size);
		}

		void MallocAllocator::Deallocate(void* ptr)
		{
			std::free(ptr);
		}
	}
}