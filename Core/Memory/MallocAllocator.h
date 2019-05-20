#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	namespace Memory
	{
		class BAROQUE_CORE_API MallocAllocator
		{
		public:
			void* Allocate(const std::size_t size);
			void Deallocate(void* ptr);
		};
	}
}
