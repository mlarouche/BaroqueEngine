#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	class BAROQUE_CORE_API VirtualMemoryAllocator
	{
	public:
		void* Allocate(const std::size_t size);
		void Deallocate(void* ptr);
	};
}