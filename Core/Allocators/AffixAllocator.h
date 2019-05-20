#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	template<typename Allocator, typename Prefix, typename Suffix = void>
	class AffixAllocator : private Allocator, private Prefix, private Suffix
	{
	public:
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