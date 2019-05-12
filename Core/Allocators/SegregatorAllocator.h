#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	template<std::size_t Threshold, typename SmallAllocator, typename LargeAllocator>
	class SegregatorAllocator : private SmallAllocator, private LargeAllocator
	{
	public:
		void* Allocate(std::size_t size)
		{
			if (size <= Threshold)
			{
				return SmallAllocator::Allocate(size);
			}
			else
			{
				return LargeAllocator::Allocate(size);
			}
		}

		void Deallocate(void* ptr)
		{
			if (ptr)
			{
				if (SmallAllocator::Owns(ptr))
				{
					SmallAllocator::Deallocate(ptr);
				}
				else
				{
					LargeAllocator::Deallocate(ptr);
				}
			}
		}

		bool Owns(void* ptr) const
		{
			return SmallAllocator::Owns(ptr) || LargeAllocator::Owns(ptr);
		}
	};
}