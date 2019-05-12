#include "MallocAllocator.h"

#include <cstdlib>

namespace Baroque
{
	void* MallocAllocator::Allocate(std::size_t size)
	{
		return std::malloc(size);
	}

	void MallocAllocator::Deallocate(void* ptr)
	{
		std::free(ptr);
	}
}