#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	namespace Memory
	{
		template<typename Allocator, typename Prefix, typename Suffix = void>
		class AffixAllocator : private Allocator, private Prefix, private Suffix
		{
		public:
			static constexpr std::size_t ArrayCapacity = Allocator::ArrayCapacity;

			void* Allocate(const std::size_t size)
			{
				Prefix::Allocate(size);
				void* result = Allocator::Allocate(size);
				Suffix::Alocate(size);
				return result;
			}

			void Deallocate(void* ptr)
			{
				Prefix::Deallocate(ptr);
				Allocator::Deallocate(ptr);
				Suffix::Deallocate(ptr);
			}

			bool Owns(void* ptr) const
			{
				return Allocator::Owns(ptr);
			}
		};
	}
}