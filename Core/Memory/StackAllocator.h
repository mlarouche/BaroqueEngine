#pragma once

#include "Core/CoreDefines.h"

#include <atomic>

namespace Baroque
{
	namespace Memory
	{
		template<std::size_t AllocSize>
		class StackAllocator
		{
		public:
			static constexpr std::size_t StackCapacity = AllocSize;

			StackAllocator()
			: _top(_storage)
			{}

			void* Allocate(const std::size_t size)
			{
				if (size > AllocSize)
				{
					return nullptr;
				}

				if ((_top + size) > (_storage + AllocSize))
				{
					return nullptr;
				}

				_previous = _top;
				_top += size;
				return _previous;
			}

			void Deallocate(void* ptr)
			{
				if (ptr == _previous)
				{
					_top = _previous;
				}
				else if (ptr == _storage)
				{
					_top = _storage;
				}
			}

			bool Owns(const void* ptr) const
			{
				return ptr >= _storage && ptr < (_storage + AllocSize);
			}

		private:
			std::uint8_t _storage[AllocSize];
			std::uint8_t* _top = nullptr;
			std::uint8_t* _previous = nullptr;
		};
	}
}