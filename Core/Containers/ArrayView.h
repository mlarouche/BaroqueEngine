#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	template<typename T>
	class ArrayView
	{
	public:
		using Value = T;
		using ConstPointer = Value const*;
		using ConstReference = Value const&;
		using SizeType = std::size_t;
		using IntegerPointerType = std::uintptr_t;

		constexpr ArrayView() = default;

		constexpr ArrayView(ConstPointer begin, ConstPointer end)
		: _begin(begin)
		, _end(end)
		{}

		constexpr ArrayView(ConstPointer begin, SizeType size)
		: _begin(begin)
		, _end(begin + size)
		{}

		template<SizeType ConstantArraySize>
		constexpr ArrayView(const T (&nativeArray)[ConstantArraySize])
		: _begin(nativeArray)
		, _end(nativeArray + ConstantArraySize)
		{}

		constexpr ArrayView(const ArrayView& copy)
		: _begin(copy._begin)
		, _end(copy._end)
		{}

		ArrayView& operator=(const ArrayView& copy)
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

		constexpr ArrayView<T> Slice(SizeType start, SizeType end) const
		{
			return ArrayView<T>(_begin + start, _begin + ((end - start) + 1));
		}

		constexpr ArrayView<T> Subset(SizeType start) const
		{
			return ArrayView<T>(_begin + start, _end);
		}

		constexpr ArrayView<T> Subset(SizeType start, SizeType count) const
		{
			return ArrayView<T>(_begin + start, count);
		}

		constexpr SizeType Size() const
		{
			return static_cast<SizeType>(_end - _begin);
		}

		constexpr ConstPointer begin() const
		{
			return _begin;
		}

		constexpr ConstPointer end() const
		{
			return _end;
		}

		constexpr ConstReference operator[](SizeType index) const
		{
			return _begin[index];
		}

	private:
		ConstPointer _begin = nullptr;
		ConstPointer _end = nullptr;
	};

	template<typename T>
	inline bool operator==(const ArrayView<T>& left, const ArrayView<T>& right)
	{
		if (left.Size() == right.Size())
		{
			auto size = left.Size();

			for (typename ArrayView<T>::SizeType i = 0; i < size; ++i)
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
	inline bool operator!=(const ArrayView<T>& left, const ArrayView<T>& right)
	{
		if (left.Size() == right.Size())
		{
			auto size = left.Size();

			for (typename ArrayView<T>::SizeType i = 0; i < size; ++i)
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
