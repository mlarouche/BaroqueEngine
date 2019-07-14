#pragma once

#include "Core/CoreDefines.h"

#include "Core/Algorithms/MinMax.h"
#include "Core/Containers/ArrayView.h"
#include "Core/Containers/ArraySpan.h"
#include "Core/Memory/Memory.h"
#include "Core/Utilities/TypeTraits.h"

#include <cstdlib>

namespace Baroque
{
	BAROQUE_EXTERN_MEMORY_CATEGORY(Array)

	class BaseArray
	{
	public:
		using SizeType = std::size_t;

		constexpr SizeType Capacity() const
		{
			return _capacity;
		}

		bool IsEmpty() const
		{
			return _size == 0;
		}

		constexpr SizeType Size() const
		{
			return _size;
		}

	protected:
		void* _data = nullptr;
		SizeType _size = 0;
		SizeType _capacity = 0;
	};

	template<typename T>
	class ArraySpan;

	template<typename T>
	class ArrayView;

	template<typename T, typename Allocator>
	class ArrayImplementation : public BaseArray, private Allocator
	{
	private:
		static constexpr const auto GrowFactor = 2.0;
		static constexpr const auto DefaultCapacity = 4;
		static constexpr const auto InitialCapacity = Allocator::StackCapacity / sizeof(T);

	public:
		using Value = T;
		using Pointer = Value *;
		using ConstPointer = Value const*;
		using Reference = Value &;
		using ConstReference = Value const&;
		using MoveReference = Value &&;

		constexpr ArrayImplementation()
		{
			if constexpr (InitialCapacity > 0)
			{
				_capacity = InitialCapacity;
				_data = allocate(InitialCapacity);
			}
		}

		constexpr explicit ArrayImplementation(SizeType initialSize)
		{
			_size = initialSize;
			_capacity = Algorithm::Max(InitialCapacity, initialSize);
			_data = allocateAndInit(_capacity, initialSize);
		}

		constexpr explicit ArrayImplementation(SizeType initialSize, ConstReference initialValue)
		{
			_size = initialSize;
			_capacity = Algorithm::Max(InitialCapacity, initialSize);
			_data = allocateAndInit(_capacity, initialSize, initialValue);
		}

		constexpr ArrayImplementation(std::initializer_list<T> initList)
		{
			_size = initList.size();
			_capacity = Algorithm::Max(InitialCapacity, _size);

			_data = allocate(_capacity);

			auto* it = Data();
			auto listIt = initList.begin();
			auto listItEnd = initList.end();

			while (listIt != listItEnd)
			{
				new (it) Value(*listIt);

				++listIt;
				++it;
			}
		}

		constexpr ArrayImplementation(const ArrayImplementation& copy)
		{
			_size = copy._size;
			_capacity = copy._capacity;

			_data = allocate(_capacity);

			this->copy(copy.Data(), Data(), _size);
		}

		template<typename OtherAllocator>
		constexpr ArrayImplementation(const ArrayImplementation<T, OtherAllocator>& copy)
		{
			_size = copy.Size();
			_capacity = copy.Capacity();

			_data = allocate(_capacity);

			this->copy(copy.Data(), Data(), _size);
		}

		template<typename = std::enable_if_t<InitialCapacity == 0>>
		constexpr ArrayImplementation(ArrayImplementation&& move)
		{
			_size = move._size;
			_capacity = move._capacity;
			_data = move._data;

			move._size = 0;
			move._capacity = 0;
			move._data = nullptr;
		}

		~ArrayImplementation()
		{
			internalDestructor();
		}

		ArrayImplementation& operator=(const ArrayImplementation& copy)
		{
			if (this != &copy)
			{
				internalDestructor();

				_size = copy._size;
				_capacity = copy._capacity;

				_data = allocate(_capacity);

				this->copy(copy.Data(), Data(), _size);
			}

			return *this;
		}

		template<typename OtherAllocator>
		ArrayImplementation& operator=(const ArrayImplementation<T, OtherAllocator>& copy)
		{
			internalDestructor();

			_size = copy.Size();
			_capacity = copy.Capacity();

			_data = allocate(_capacity);

			this->copy(copy.Data(), Data(), _size);

			return *this;
		}

		template<typename = std::enable_if_t<InitialCapacity == 0>>
		ArrayImplementation& operator=(ArrayImplementation&& move)
		{
			internalDestructor();

			_size = move._size;
			_capacity = move._capacity;
			_data = move._data;

			move._size = 0;
			move._capacity = 0;
			move._data = nullptr;

			return *this;
		}

		void Add(ConstReference item)
		{
			ensureCapacity();

			auto* newItem = Data() + _size;

			new (newItem) Value(item);

			++_size;
		}

		void Add(MoveReference item)
		{
			ensureCapacity();

			auto* newItem = Data() + _size;

			new (newItem) Value(std::forward<Value>(item));

			++_size;
		}

		void Add(std::initializer_list<Value> initList)
		{
			auto initListSize = initList.size();

			auto newCapacity = Algorithm::Max(_size + initListSize, static_cast<SizeType>(static_cast<double>(_capacity) * GrowFactor));
			Reserve(newCapacity);

			copy(initList.begin(), Data() + _size, initListSize);

			_size += initListSize;
		}

		void Add(ArrayView<T> view)
		{
			auto viewSize = view.Size();

			auto newCapacity = Algorithm::Max(_size + viewSize, static_cast<SizeType>(static_cast<double>(_capacity) * GrowFactor));
			Reserve(newCapacity);

			copy(view.begin(), Data() + _size, viewSize);

			_size += viewSize;
		}

		void Add(ArraySpan<T> view)
		{
			auto viewSize = view.Size();

			auto newCapacity = Algorithm::Max(_size + viewSize, static_cast<SizeType>(static_cast<double>(_capacity) * GrowFactor));
			Reserve(newCapacity);

			copy(view.begin(), Data() + _size, viewSize);

			_size += viewSize;
		}

		bool AddUnique(ConstReference value)
		{
			if (!Contains(value))
			{
				Add(value);

				return true;
			}

			return false;
		}

		bool AddUnique(MoveReference value)
		{
			if (!Contains(value))
			{
				Add(std::forward<Value>(value));

				return true;
			}

			return false;
		}

		void Clear()
		{
			destroyItems();

			_size = 0;
		}

		bool Contains(ConstReference value) const
		{
			for (auto& entry : *this)
			{
				if (entry == value)
				{
					return true;
				}
			}

			return false;
		}

		template<typename Predicate>
		bool ContainsByPredicate(const Predicate& predicate) const
		{
			for (auto& entry : *this)
			{
				if (predicate(entry))
				{
					return true;
				}
			}

			return false;
		}

		constexpr Pointer Data()
		{
			return reinterpret_cast<Pointer>(_data);
		}

		constexpr ConstPointer Data() const
		{
			return reinterpret_cast<ConstPointer>(_data);
		}

		template<typename... Args>
		void Emplace(Args&&... args)
		{
			ensureCapacity();

			auto* newItem = Data() + _size;

			new (newItem) Value(std::forward<Args>(args)...);

			++_size;
		}

		template<typename... Args>
		void EmplaceAt(SizeType index, Args&&... args)
		{
			ensureCapacity();

			moveRight(Data(), index, _size, 1);

			auto* insertedItem = Data() + index;

			new (insertedItem) Value(std::forward<Args>(args)...);

			++_size;
		}

		Pointer Erase(ConstPointer position)
		{
			RemoveAt(position - Data());
			return const_cast<Pointer>(position);
		}

		Pointer Find(ConstReference value)
		{
			auto* it = begin();
			auto* itEnd = end();

			while (it != itEnd)
			{
				if (*it == value)
				{
					return it;
				}

				++it;
			}

			return nullptr;
		}

		ConstPointer Find(ConstReference value) const
		{
			auto* it = begin();
			auto* itEnd = end();

			while (it != itEnd)
			{
				if (*it == value)
				{
					return it;
				}

				++it;
			}

			return nullptr;
		}

		template<typename Predicate>
		Pointer FindByPredicate(const Predicate& predicate)
		{
			auto* it = begin();
			auto* itEnd = end();

			while (it != itEnd)
			{
				if (predicate(*it))
				{
					return it;
				}

				++it;
			}

			return nullptr;
		}

		template<typename Predicate>
		ConstPointer FindByPredicate(const Predicate& predicate) const
		{
			auto* it = begin();
			auto* itEnd = end();

			while (it != itEnd)
			{
				if (predicate(*it))
				{
					return it;
				}

				++it;
			}

			return nullptr;
		}

		Reference Front()
		{
			return *begin();
		}

		ConstReference Front() const
		{
			return *begin();
		}

		SizeType IndexOf(ConstReference value) const
		{
			auto* data = Data();

			for (SizeType index = 0; index < _size; ++index)
			{
				if (data[index] == value)
				{
					return index;
				}
			}

			return _size;
		}

		template<typename Predicate>
		SizeType IndexOfByPredicate(const Predicate& predicate) const
		{
			auto* data = Data();

			for (SizeType index = 0; index < _size; ++index)
			{
				if (predicate(data[index]))
				{
					return index;
				}
			}

			return _size;
		}

		void Insert(SizeType index, ConstReference value)
		{
			ensureCapacity();

			moveRight(Data(), index, _size, 1);

			auto* insertedItem = Data() + index;

			new (insertedItem) Value(value);

			++_size;
		}

		void Insert(SizeType index, MoveReference value)
		{
			ensureCapacity();

			moveRight(Data(), index, _size, 1);

			auto* insertedItem = Data() + index;

			new (insertedItem) Value(std::forward<Value>(value));

			++_size;
		}

		void Insert(SizeType index, std::initializer_list<Value> initList)
		{
			auto initListSize = initList.size();

			auto newCapacity = Algorithm::Max(_size + initListSize, static_cast<SizeType>(static_cast<double>(_capacity) * GrowFactor));
			Reserve(newCapacity);

			moveRight(Data(), index, _size, initListSize);

			copy(initList.begin(), Data() + index, initListSize);

			_size += initListSize;
		}

		void Insert(SizeType index, ArrayView<T> view)
		{
			auto viewSize = view.Size();

			auto newCapacity = Algorithm::Max(_size + viewSize, static_cast<SizeType>(static_cast<double>(_capacity) * GrowFactor));
			Reserve(newCapacity);

			moveRight(Data(), index, _size, viewSize);

			copy(view.begin(), Data() + index, viewSize);

			_size += viewSize;
		}

		void Insert(SizeType index, ArraySpan<T> span)
		{
			auto spanSize = span.Size();

			auto newCapacity = Algorithm::Max(_size + spanSize, static_cast<SizeType>(static_cast<double>(_capacity) * GrowFactor));
			Reserve(newCapacity);

			moveRight(Data(), index, _size, spanSize);

			copy(span.begin(), Data() + index, spanSize);

			_size += spanSize;
		}

		Reference Last()
		{
			return *(end() - 1);
		}

		ConstReference Last() const
		{
			return *(end() - 1);
		}

		void Push(ConstReference value)
		{
			Add(value);
		}

		void Push(MoveReference value)
		{
			Add(std::forward<Value>(value));
		}

		Value Pop()
		{
			auto returnValue = Top();

			RemoveAt(_size - 1);

			return returnValue;
		}

		SizeType Remove(ConstReference value)
		{
			SizeType removed = 0;

			auto* data = Data();

			for (SizeType index = _size - 1; index >= 0 && index < _size; --index)
			{
				if (data[index] == value)
				{
					if constexpr (!std::is_trivially_destructible_v<Value>)
					{
						data[index].~Value();
					}

					moveLeft(data, index, _size);

					--_size;
					++removed;
				}
			}

			return removed;
		}

		template<typename Predicate>
		SizeType RemoveByPredicate(const Predicate& predicate)
		{
			SizeType removed = 0;

			auto* data = Data();

			for (SizeType index = _size - 1; index >= 0 && index < _size; --index)
			{
				if (predicate(data[index]))
				{
					if constexpr (!std::is_trivially_destructible_v<Value>)
					{
						data[index].~Value();
					}

					moveLeft(data, index, _size);

					--_size;
					++removed;
				}
			}

			return removed;
		}

		void RemoveAt(SizeType index)
		{
			if (index < _size)
			{
				auto* data = Data();

				if constexpr (!std::is_trivially_destructible_v<Value>)
				{
					data[index].~Value();
				}

				moveLeft(data, index, _size);

				--_size;
			}
		}

		void Reserve(SizeType newCapacity)
		{
			if (newCapacity > _capacity)
			{
				auto* newData = allocate(newCapacity);

				if (_data)
				{
					relocate(reinterpret_cast<Pointer>(_data), reinterpret_cast<Pointer>(newData), _size);

					deallocateData();
				}

				_data = newData;
				_capacity = newCapacity;
			}
		}

		void Resize(SizeType newSize)
		{
			if (newSize > _size)
			{
				if (newSize > _capacity)
				{
					Reserve(newSize);
				}

				init(Data() + _size, newSize - _size);

				_size = newSize;
			}
			else if (newSize < _size)
			{
				destroyItems(begin() + newSize, end());
				_size = newSize;
			}
		}

		constexpr ArraySpan<T> Slice(SizeType start, SizeType end)
		{
			return ArraySpan<T>(Data() + start, Data() + ((end - start) + 1));
		}

		constexpr ArrayView<T> Slice(SizeType start, SizeType end) const
		{
			return ArrayView<T>(Data() + start, Data() + ((end - start) + 1));
		}

		constexpr ArraySpan<T> Subset(SizeType start)
		{
			return ArraySpan<T>(Data() + start, end());
		}

		constexpr ArrayView<T> Subset(SizeType start) const
		{
			return ArrayView<T>(Data() + start, end());
		}

		constexpr ArraySpan<T> Subset(SizeType start, SizeType count)
		{
			return ArraySpan<T>(Data() + start, count);
		}

		constexpr ArrayView<T> Subset(SizeType start, SizeType count) const
		{
			return ArrayView<T>(Data() + start, count);
		}

		ArrayView<T> ToArrayView() const
		{
			return ArrayView<T>(Data(), _size);
		}

		ArraySpan<T> ToArraySpan()
		{
			return ArraySpan<T>(Data(), _size);
		}

		Reference Top()
		{
			return operator[](_size - 1);
		}

		ConstReference Top() const
		{
			return operator[](_size - 1);
		}

		constexpr Pointer begin()
		{
			return Data();
		}

		constexpr ConstPointer begin() const
		{
			return Data();
		}

		constexpr Pointer end()
		{
			return Data() + _size;
		}

		constexpr ConstPointer end() const
		{
			return Data() + _size;
		}

		constexpr Reference operator[](SizeType index)
		{
			return *(Data() + index);
		}

		constexpr ConstReference operator[](SizeType index) const
		{
			return *(Data() + index);
		}

	private:
		Pointer allocate(SizeType itemCount)
		{
			return static_cast<Pointer>(BAROQUE_ALLOC((*this), itemCount * sizeof(Value), Array));
		}

		void copy(ConstPointer source, Pointer destination, SizeType count)
		{
			if constexpr (std::is_trivially_copyable_v<Value>)
			{
				std::memcpy(destination, source, sizeof(Value) * count);
			}
			else
			{
				auto sourceIt = source;
				auto destinationIt = destination;

				for (SizeType i = 0; i < count; ++i, ++sourceIt, ++destinationIt)
				{
					new (destinationIt) Value(*sourceIt);
				}
			}
		}

		void internalDestructor()
		{
			destroyItems();
			deallocateData();
		}

		void destroyItems()
		{
			if constexpr (!std::is_trivially_destructible_v<Value>)
			{
				destroyItems(begin(), end());
			}
		}

		void destroyItems(Pointer it, Pointer end)
		{
			if constexpr (!std::is_trivially_destructible_v<Value>)
			{
				for (; it != end; ++it)
				{
					it->~Value();
				}
			}
			else
			{
				BAROQUE_UNUSED(it);
				BAROQUE_UNUSED(end);
			}
		}

		void deallocateData()
		{
			if (_data)
			{
				this->Deallocate(_data);
			}
		}

		void ensureCapacity()
		{
			if (_size == _capacity)
			{
				Reserve(_capacity ? static_cast<SizeType>(static_cast<double>(_capacity) * GrowFactor) : DefaultCapacity);
			}
		}

		constexpr void init(Pointer memory, SizeType itemCount, ConstReference initValue)
		{
			auto* it = memory;

			for (SizeType i = 0; i < itemCount; ++i, ++it)
			{
				new (it) Value(initValue);
			}
		}

		constexpr void init(Pointer memory, SizeType itemCount)
		{
			if constexpr (std::is_trivially_constructible_v<Value>)
			{
				std::memset(memory, 0, itemCount * sizeof(Value));
			}
			else
			{
				auto* it = memory;

				for (SizeType i = 0; i < itemCount; ++i, ++it)
				{
					new (it) Value();
				}
			}
		}

		void relocate(Pointer oldData, Pointer newData, SizeType itemCount)
		{
			if constexpr (Traits::IsTriviallyRelocatable_v<Value>)
			{
				std::memcpy(newData, oldData, sizeof(Value) * itemCount);
			}
			else
			{
				auto sourceIt = oldData;
				auto destinationIt = newData;

				for (SizeType i = 0; i < itemCount; ++i, ++sourceIt, ++destinationIt)
				{
					new (destinationIt) Value(std::move(*sourceIt));
				}
			}
		}

		void moveLeft(Pointer data, SizeType start, SizeType end)
		{
			auto* sourceIt = data + start + 1;
			auto* destinationIt = data + start;
			auto* sourceItEnd = data + end;

			for (; sourceIt != sourceItEnd; ++sourceIt, ++destinationIt)
			{
				new (destinationIt) Value(std::move(*sourceIt));
			}
		}

		void moveRight(Pointer data, SizeType start, SizeType end, SizeType step)
		{
			auto* sourceIt = data + (end - 1);
			auto* destinationIt = data + (end) + (step-1);

			auto* sourceItEnd = data + (start - 1);

			for (; sourceIt != sourceItEnd; --sourceIt, --destinationIt)
			{
				new (destinationIt) Value(std::move(*sourceIt));
			}
		}

		Pointer allocateAndInit(SizeType allocationCount, SizeType initCount, ConstReference initValue)
		{
			auto* memory = allocate(allocationCount);
			init(memory, initCount, initValue);
			return memory;
		}

		Pointer allocateAndInit(SizeType allocationCount, SizeType initCount)
		{
			auto* memory = allocate(allocationCount);
			init(memory, initCount);
			return memory;
		}
	};

	template<typename T, typename Allocator>
	inline bool operator==(const ArrayImplementation<T, Allocator>& left, const ArrayImplementation<T, Allocator>& right)
	{
		if (left.Size() == right.Size())
		{
			for (decltype(left.Size())  index = 0; index < left.Size(); ++index)
			{
				if (!(left[index] == right[index]))
				{
					return false;
				}
			}

			return true;
		}

		return false;
	}

	template<typename T, typename Allocator>
	inline bool operator!=(const ArrayImplementation<T, Allocator>& left, const ArrayImplementation<T, Allocator>& right)
	{
		if (left.Size() == right.Size())
		{
			for (decltype(left.Size()) index = 0; index < left.Size(); ++index)
			{
				if (left[index] != right[index])
				{
					return true;
				}
			}

			return false;
		}

		return true;
	}

	template<typename T, typename Allocator = Baroque::Memory::DefaultAllocator>
	using Array = ArrayImplementation<T, Allocator>;

	template<typename T, std::size_t Size>
	using SmallArray = ArrayImplementation<T, Baroque::Memory::SmallAllocator<Size * sizeof(T)>>;
}