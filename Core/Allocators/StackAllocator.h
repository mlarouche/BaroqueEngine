#pragma once

#include "Core/CoreDefines.h"

#include <atomic>

namespace Baroque
{
	template<std::size_t AllocSize>
	class StackAllocator 
	{
	public:
		StackAllocator()
			: _top(_storage)
		{}

		void* Allocate(std::size_t size)
		{
			if (size > AllocSize)
			{
				return nullptr;
			}

			if ((_top + size) > (_storage + AllocSize))
			{
				return nullptr;
			}

			void* result = _top;
			_top += size;
			return result;
		}

		void Deallocate(void*)
		{
		}

		bool Owns(void* ptr) const
		{
			return ptr >= _storage && ptr < (_storage + AllocSize);
		}

	private:
		std::uint8_t _storage[AllocSize];
		std::uint8_t* _top = nullptr;
	};
}