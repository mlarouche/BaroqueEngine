#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	template<typename T, typename BackendAllocator>
	class ObjectAllocator : private BackendAllocator
	{
	public:
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