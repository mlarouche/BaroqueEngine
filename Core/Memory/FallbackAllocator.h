#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	namespace Memory
	{
		template<typename Primary, typename Fallback>
		class FallbackAllocator : private Primary, private Fallback
		{
		public:
			static constexpr std::size_t ArrayCapacity = Primary::ArrayCapacity;

			void* Allocate(const std::size_t size)
			{
				void* result = Primary::Allocate(size);
				if (!result)
				{
					result = Fallback::Allocate(size);
				}
				return result;
			}

			void Deallocate(void* ptr)
			{
				if (ptr)
				{
					if (Primary::Owns(ptr))
					{
						Primary::Deallocate(ptr);
					}
					else
					{
						Fallback::Deallocate(ptr);
					}
				}
			}

			bool Owns(const void* ptr) const
			{
				return Primary::Owns(ptr) || Fallback::Owns(ptr);
			}
		};
	}
}