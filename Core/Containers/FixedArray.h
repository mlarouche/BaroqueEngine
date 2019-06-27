#pragma once

#include "Core/CoreDefines.h"

#include "Core/Containers/ArraySpan.h"
#include "Core/Containers/ArrayView.h"

#include <type_traits>

namespace Baroque
{
	template<typename T, std::size_t ArraySize>
	class FixedArray
	{
	public:
		using Value = T;
		using Pointer = Value *;
		using ConstPointer = Value const*;
		using Reference = Value &;
		using ConstReference = Value const&;
		using SizeType = std::size_t;

		constexpr FixedArray()
		{
			if constexpr (std::is_trivial_v<T>)
			{
				std::memset(_array, 0, sizeof(_array));
			}
		}

		constexpr FixedArray(std::initializer_list<T> ilist)
		: FixedArray()
		{
			if (ilist.size() <= ArraySize)
			{
				if constexpr (std::is_trivial_v<T>)
				{
					std::memcpy(_array, ilist.begin(), sizeof(T) * ilist.size());
				}
				else
				{
					for (SizeType i = 0; i < ilist.size(); ++i)
					{
						new (&_array[i]) Value(ilist.begin()[i]);
					}
				}
			}
		}

		constexpr FixedArray(const FixedArray& copy)
		{
			if constexpr (std::is_trivial_v<T>)
			{
				std::memcpy(_array, copy._array, sizeof(_array));
			}
			else
			{
				for (SizeType i = 0; i < ArraySize; ++i)
				{
					new (&_array[i]) Value(copy._array[i]);
				}
			}
		}

		constexpr FixedArray(FixedArray&& move)
		{
			if constexpr (std::is_trivial_v<T>)
			{
				std::memcpy(_array, move._array, sizeof(_array));
			}
			else
			{
				for (SizeType i = 0; i < ArraySize; ++i)
				{
					new (&_array[i]) Value(std::move(move._array[i]));
				}
			}
		}

		constexpr FixedArray& operator=(const FixedArray& copy)
		{
			if (this != &copy)
			{
				if constexpr (std::is_trivial_v<T>)
				{
					std::memcpy(_array, copy._array, sizeof(_array));
				}
				else
				{
					for (SizeType i = 0; i < ArraySize; ++i)
					{
						new (&_array[i]) Value(copy._array[i]);
					}
				}
			}

			return *this;
		}

		constexpr FixedArray& operator=(FixedArray&& move)
		{
			if constexpr (std::is_trivial_v<T>)
			{
				std::memcpy(_array, move._array, sizeof(_array));
			}
			else
			{
				for (SizeType i = 0; i < ArraySize; ++i)
				{
					new (&_array[i]) Value(std::move(move._array[i]));
				}
			}

			return *this;
		}

		constexpr FixedArray& operator=(std::initializer_list<T> ilist)
		{
			if (ilist.size() <= ArraySize)
			{
				if constexpr (std::is_trivial_v<T>)
				{
					std::memcpy(_array, ilist.begin(), sizeof(T) * ilist.size());
				}
				else
				{
					for (SizeType i = 0; i < ilist.size(); ++i)
					{
						new (&_array[i]) Value(ilist.begin()[i]);
					}
				}
			}

			return *this;
		}

		constexpr bool Contains(ConstReference value) const
		{
			for (SizeType i = 0; i < ArraySize; ++i)
			{
				if (_array[i] == value)
				{
					return true;
				}
			}

			return false;
		}

		constexpr Pointer Data()
		{
			return _array;
		}

		constexpr ConstPointer Data() const
		{
			return _array;
		}

		constexpr void DeleteAll()
		{
			if constexpr (std::is_pointer_v<T>)
			{
				for (SizeType i = 0; i < ArraySize; ++i)
				{
					delete _array[i];
				}
			}
		}

		constexpr SizeType IndexOf(ConstReference value) const
		{
			for (SizeType i = 0; i < ArraySize; ++i)
			{
				if (_array[i] == value)
				{
					return i;
				}
			}

			return ArraySize;
		}

		constexpr ArraySpan<T> Slice(SizeType start, SizeType end)
		{
			return ArraySpan<T>(_array + start, _array + ((end - start) + 1));
		}

		constexpr ArrayView<T> Slice(SizeType start, SizeType end) const
		{
			return ArrayView<T>(_array + start, _array + ((end - start) + 1));
		}

		constexpr ArraySpan<T> Subset(SizeType start)
		{
			return ArraySpan<T>(_array + start, _array + ArraySize);
		}

		constexpr ArrayView<T> Subset(SizeType start) const
		{
			return ArrayView<T>(_array + start, _array + ArraySize);
		}

		constexpr ArraySpan<T> Subset(SizeType start, SizeType count)
		{
			return ArraySpan<T>(_array + start, count);
		}

		constexpr ArrayView<T> Subset(SizeType start, SizeType count) const
		{
			return ArrayView<T>(_array + start, count);
		}

		constexpr SizeType Size() const
		{
			return ArraySize;
		}

		constexpr ArrayView<T> ToArrayView() const
		{
			return ArrayView<T>(_array, _array + ArraySize);
		}

		constexpr ArraySpan<T> ToArraySpan()
		{
			return ArraySpan<T>(_array, _array + ArraySize);
		}

		constexpr Pointer begin()
		{
			return _array;
		}

		constexpr ConstPointer begin() const
		{
			return _array;
		}

		constexpr Pointer end()
		{
			return _array + ArraySize;
		}

		constexpr ConstPointer end() const
		{
			return _array + ArraySize;
		}

		constexpr ConstReference operator[](SizeType index) const
		{
			return _array[index];
		}

		constexpr Reference operator[](SizeType index)
		{
			return _array[index];
		}

	private:
		T _array[ArraySize];
	};

	template<typename T, std::size_t ArraySize>
	inline bool operator==(const FixedArray<T, ArraySize>& left, const FixedArray<T, ArraySize>& right)
	{
		for (std::size_t i = 0; i < ArraySize; ++i)
		{
			if (!(left[i] == right[i]))
			{
				return false;
			}
		}

		return true;
	}

	template<typename T, std::size_t ArraySize>
	inline bool operator!=(const FixedArray<T, ArraySize>& left, const FixedArray<T, ArraySize>& right)
	{
		for (std::size_t i = 0; i < ArraySize; ++i)
		{
			if (left[i] != right[i])
			{
				return true;
			}
		}

		return false;
	}

	template<typename... Types>
	constexpr Baroque::FixedArray<std::common_type_t<Types...>, sizeof...(Types)> MakeFixedArray(Types&& ...value)
	{
		return { std::forward<Types>(value)... };
	}
}