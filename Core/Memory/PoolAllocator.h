#pragma once

#include "Core/CoreDefines.h"

#include "Core/Memory/ObjectAllocator.h"

#include <cstring>
#include <utility>

namespace Baroque
{
	namespace Memory
	{
		template<typename BackendAllocator, std::size_t EntrySize, std::size_t PreAllocCount = 256>
		class PoolAllocator : private BackendAllocator
		{
		public:
			static_assert(EntrySize >= sizeof(void*), "Entry size must be greater or equal than a pointer size");

			PoolAllocator()
			{
				allocateMemoryBlock();
			}

			~PoolAllocator()
			{
				void* it = _blockAllocList;

				while (it)
				{
					void* next = *((void**)it);

					BackendAllocator::Deallocate(it);

					it = next;
				}
			}

			void* Allocate(const std::size_t size = 0)
			{
				BAROQUE_UNUSED(size);

				if (!_free)
				{
					allocateMemoryBlock();
				}

				void* result = _free;
				_free = *((void**)_free);
				return result;
			}

			void Deallocate(void* ptr)
			{
				*(void**)ptr = _free;
				_free = ptr;
			}

		private:
			void allocateMemoryBlock()
			{
				std::uint8_t* memoryBlock = reinterpret_cast<std::uint8_t*>(BackendAllocator::Allocate(EntrySize * PreAllocCount + sizeof(void*)));

				*(void**)memoryBlock = _blockAllocList;
				_blockAllocList = memoryBlock;

				memoryBlock += sizeof(std::uint8_t*);

				for (std::size_t i = 0; i < PreAllocCount; ++i)
				{
					Deallocate(memoryBlock);
					memoryBlock += EntrySize;
				}
			}

		private:
			void* _blockAllocList = nullptr;
			void* _free = nullptr;
		};

		template<typename T, typename BackendAllocator, std::size_t PreAllocCount = 128>
		using PoolObjectAllocator = Baroque::Memory::ObjectAllocator<T, PoolAllocator<BackendAllocator, sizeof(T), PreAllocCount>>;
	}
}