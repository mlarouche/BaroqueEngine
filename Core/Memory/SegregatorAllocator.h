#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	namespace Memory
	{
		template<std::size_t Threshold, typename SmallAllocator, typename LargeAllocator>
		class SegregatorAllocator : private SmallAllocator, private LargeAllocator
		{
		public:
			static constexpr std::size_t StackCapacity = 0;

			void* Allocate(const std::size_t size)
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

			bool Owns(const void* ptr) const
			{
				return SmallAllocator::Owns(ptr) || LargeAllocator::Owns(ptr);
			}
		};
	}
}