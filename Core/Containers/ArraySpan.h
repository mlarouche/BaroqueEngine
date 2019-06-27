#pragma once

#include "Core/CoreDefines.h"

#include "Core/Containers/ArrayView.h"

namespace Baroque
{
	template<typename T>
	class ArraySpan
	{
	public:
		using Value = T;
		using Pointer = Value*;
		using ConstPointer = Value const*;
		using Reference = Value &;
		using ConstReference = Value const&;
		using SizeType = std::size_t;
		using IntegerPointerType = std::uintptr_t;

		constexpr ArraySpan() = default;

		constexpr ArraySpan(Pointer begin, Pointer end)
		: _begin(begin)
		, _end(end)
		{}

		constexpr ArraySpan(Pointer begin, SizeType size)
		: _begin(begin)
		, _end(begin + size)
		{}

		template<SizeType ConstantArraySize>
		constexpr ArraySpan(T (&nativeArray)[ConstantArraySize])
		: _begin(nativeArray)
		, _end(nativeArray + ConstantArraySize)
		{}

		constexpr ArraySpan(const ArraySpan& copy)
		: _begin(copy._begin)
		, _end(copy._end)
		{}

		ArraySpan& operator=(const ArraySpan& copy)
		{
			if (this != &copy)
			{
				_begin = copy._begin;
				_end = copy._end;
			}

			return *this;
		}

		constexpr bool Contains(ConstReference value) const
		{
			auto it = _begin;
			while (it != _end)
			{
				if (*it == value)
				{
					return true;
				}

				++it;
			}

			return false;
		}

		constexpr Pointer Data()
		{
			return _begin;
		}

		constexpr ConstPointer Data() const
		{
			return _begin;
		}

		constexpr SizeType IndexOf(ConstReference value) const
		{
			auto it = _begin;
			SizeType index = 0;

			while (it != _end)
			{
				if (*it == value)
				{
					return index;
				}

				++it;
				++index;
			}

			return Size();
		}

		constexpr ArraySpan<T> Slice(SizeType start, SizeType end)
		{
			return ArraySpan<T>(_begin + start, _begin + ((end - start) + 1));
		}

		constexpr ArrayView<T> Slice(SizeType start, SizeType end) const
		{
			return ArrayView<T>(_begin + start, _begin + ((end - start) + 1));
		}

		constexpr ArraySpan<T> Subset(SizeType start)
		{
			return ArraySpan<T>(_begin + start, _end);
		}

		constexpr ArrayView<T> Subset(SizeType start) const
		{
			return ArrayView<T>(_begin + start, _end);
		}

		constexpr ArraySpan<T> Subset(SizeType start, SizeType count)
		{
			return ArraySpan<T>(_begin + start, count);
		}

		constexpr ArrayView<T> Subset(SizeType start, SizeType count) const
		{
			return ArrayView<T>(_begin + start, count);
		}

		constexpr SizeType Size() const
		{
			return static_cast<SizeType>(_end - _begin);
		}

		constexpr Pointer begin()
		{
			return _begin;
		}

		constexpr ConstPointer begin() const
		{
			return _begin;
		}

		constexpr Pointer end()
		{
			return _end;
		}

		constexpr ConstPointer end() const
		{
			return _end;
		}

		constexpr Reference operator[](SizeType index)
		{
			return _begin[index];
		}

		constexpr ConstReference operator[](SizeType index) const
		{
			return _begin[index];
		}

	private:
		Pointer _begin = nullptr;
		Pointer _end = nullptr;
	};

	template<typename T>
	inline bool operator==(const ArraySpan<T>& left, const ArraySpan<T>& right)
	{
		if (left.Size() == right.Size())
		{
			auto size = left.Size();

			for (typename ArraySpan<T>::SizeType i = 0; i < size; ++i)
			{
				if (!(left[i] == right[i]))
				{
					return false;
				}
			}

			return true;
		}

		return false;
	}

	template<typename T>
	inline bool operator!=(const ArraySpan<T>& left, const ArraySpan<T>& right)
	{
		if (left.Size() == right.Size())
		{
			auto size = left.Size();

			for (typename ArraySpan<T>::SizeType i = 0; i < size; ++i)
			{
				if (left[i] != right[i])
				{
					return true;
				}
			}

			return false;
		}

		return true;
	}
}