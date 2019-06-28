#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	namespace Memory
	{
		template<typename T, typename BackendAllocator>
		class ObjectAllocator : private BackendAllocator
		{
		public:
			static constexpr std::size_t ArrayCapacity = 0;

			template<typename... Args>
			T* Allocate(Args&& ... args)
			{
				void* allocation = BackendAllocator::Allocate(sizeof(T));
				new (allocation) T(std::forward<Args>(args)...);
				return static_cast<T*>(allocation);
			}

			void Deallocate(T* ptr)
			{
				ptr->~T();
				BackendAllocator::Deallocate(ptr);
			}
		};
	}
}