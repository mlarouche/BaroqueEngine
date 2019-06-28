#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	namespace Memory
	{
		class BAROQUE_CORE_API VirtualMemoryAllocator
		{
		public:
			static constexpr std::size_t ArrayCapacity = 0;

			void* Allocate(const std::size_t size);
			void Deallocate(void* ptr);
		};
	}
}