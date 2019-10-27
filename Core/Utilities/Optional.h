#pragma once

#include "Core/CoreDefines.h"

#include "Core/Utilities/TypeTraits.h"

namespace Baroque
{
	struct InPlaceType {};
	inline constexpr InPlaceType InPlace{};

	template<typename T>
	class Optional
	{
	public:
		using ValueType = T;

		constexpr Optional()
		{
		}

		constexpr Optional(const ValueType& value)
		: _isUsed(true)
		, _value(value)
		{}

		constexpr Optional(ValueType&& value)
		: _isUsed(true)
		, _value(std::forward<T>(value))
		{}

		constexpr Optional(const Optional& copy)
		: _isUsed(copy._isUsed)
		{
			if (_isUsed)
			{
				new (&_value) ValueType(copy._value);
			}
		}

		constexpr Optional(Optional&& move)
		: _isUsed(move._isUsed)
		{
			move._isUsed = false;

			if (_isUsed)
			{
				new (&_value) ValueType(std::forward<ValueType>(move._value));
			}
		}

		template<typename... Args>
		constexpr Optional(InPlaceType, Args&& ... args)
		: _isUsed(true)
		{
			new (&_value) ValueType(std::forward<Args>(args)...);
		}

		~Optional()
		{
			if constexpr (!std::is_trivially_destructible_v<T>)
			{
				destroy();
			}
		}

		constexpr Optional& operator=(const Optional& copy)
		{
			if (this != &copy)
			{
				destroy();

				_isUsed = copy._isUsed;

				if (_isUsed)
				{
					new (&_value) ValueType(copy._value);
				}
			}

			return *this;
		}

		constexpr Optional& operator=(Optional&& move)
		{
			destroy();

			_isUsed = move._isUsed;

			if (_isUsed)
			{
				new (&_value) ValueType(std::forward<ValueType>(move._value));
			}

			move._isUsed = false;

			return *this;
		}

		constexpr Optional& operator=(const ValueType& value)
		{
			destroy();

			new (&_value) ValueType(value);
			_isUsed = true;

			return *this;
		}

		constexpr Optional& operator=(ValueType&& value)
		{
			destroy();

			new (&_value) ValueType(std::forward<ValueType>(value));
			_isUsed = true;

			return *this;
		}

		constexpr Optional& operator=(std::nullptr_t)
		{
			Clear();
			return *this;
		}

		constexpr void Clear()
		{
			destroy();

			_isUsed = false;
		}

		constexpr bool IsValid() const
		{
			return _isUsed;
		}

		template<typename... Args>
		constexpr void Emplace(Args&&... args)
		{
			destroy();

			new (&_value) ValueType(std::forward<Args>(args)...);

			_isUsed = true;
		}

		constexpr ValueType& Value()
		{
			return _value;
		}

		constexpr const ValueType& Value() const
		{
			return _value;
		}

		constexpr ValueType ValueOr(ValueType&& defaultValue) const&
		{
			return IsValid() ? _value : std::forward<ValueType>(defaultValue);
		}

		constexpr ValueType ValueOr(ValueType&& defaultValue) &&
		{
			return IsValid() ? std::move(_value) : std::forward<ValueType>(defaultValue);
		}

		constexpr explicit operator bool() const
		{
			return IsValid();
		}

		constexpr ValueType& operator*()
		{
			return _value;
		}

		constexpr const ValueType& operator*() const
		{
			return _value;
		}

		constexpr ValueType* operator->()
		{
			return &_value;
		}

		constexpr const ValueType* operator->() const
		{
			return &_value;
		}

	private:
		constexpr void destroy()
		{
			if constexpr (!std::is_trivially_destructible_v<T>)
			{
				if (_isUsed)
				{
					_value.~ValueType();
				}
			}
		}

	private:
		// mlarouche - The union is used to disable the automatic call to a
		// complex type ctor/dtor. It allows defaut-constructed Optional of
		// a complex type to be near no-op.
		union
		{
			T _value;
		};

		bool _isUsed = false;
	};

	template<typename T>
	constexpr inline bool operator==(const Optional<T>& left, const Optional<T>& right)
	{
		if (left.IsValid() == right.IsValid())
		{
			if (left.IsValid())
			{
				return left.Value() == right.Value();
			}

			return true;
		}

		return false;
	}

	template<typename T>
	constexpr inline bool operator!=(const Optional<T>& left, const Optional<T>& right)
	{
		return !operator==(left, right);
	}

	template<typename T>
	constexpr inline bool operator<(const Optional<T>& left, const Optional<T>& right)
	{
		if (left.IsValid() == right.IsValid())
		{
			if (left.IsValid())
			{
				return left.Value() < right.Value();
			}

			return true;
		}

		return false;
	}

	template<typename T>
	constexpr inline bool operator>(const Optional<T>& left, const Optional<T>& right)
	{
		if (left.IsValid() == right.IsValid())
		{
			if (left.IsValid())
			{
				return left.Value() > right.Value();
			}

			return true;
		}

		return false;
	}

	template<typename T>
	constexpr inline bool operator<=(const Optional<T>& left, const Optional<T>& right)
	{
		if (left.IsValid() == right.IsValid())
		{
			if (left.IsValid())
			{
				return left.Value() <= right.Value();
			}

			return true;
		}

		return false;
	}

	template<typename T>
	constexpr inline bool operator>=(const Optional<T>& left, const Optional<T>& right)
	{
		if (left.IsValid() == right.IsValid())
		{
			if (left.IsValid())
			{
				return left.Value() >= right.Value();
			}

			return true;
		}

		return false;
	}
}