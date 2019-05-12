#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	class MallocAllocator
	{
	public:
		void* Allocate(std::size_t size);
		void Deallocate(void* ptr);
	};
}