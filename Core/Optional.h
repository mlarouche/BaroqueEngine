#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	struct InPlaceType {};
	inline constexpr InPlaceType InPlace{};

	template<typename T>
	class Optional
	{
	public:
		using ValueType = T;

		Optional()
		: _dummy('\0')
		{
		}

		Optional(ValueType&& value)
		: _value(std::forward<ValueType>(value))
		, _isUsed(true)
		{
		}

		template<typename... Args>
		Optional(InPlaceType, Args&& ... args)
		: _isUsed(true)
		{
			new (&_value) ValueType(std::forward<Args>(args)...);
		}

		Optional(const Optional& copy)
		: _isUsed(copy._isUsed)
		{
			if (_isUsed)
			{
				new (&_value) ValueType(copy._value);
			}
		}

		Optional(Optional&& move)
		: _isUsed(move._isUsed)
		{
			move._isUsed = false;

			if (_isUsed)
			{
				new (&_value) ValueType(std::forward<ValueType>(move._value));
			}
		}

		~Optional()
		{
			if (_isUsed)
			{
				_value.~ValueType();
			}
		}

		Optional& operator=(const Optional& copy)
		{
			if (this != &copy)
			{
				if (_isUsed)
				{
					_value.~ValueType();
				}

				_isUsed = copy._isUsed;

				if (_isUsed)
				{
					new (&_value) ValueType(copy._value);
				}
			}

			return *this;
		}

		Optional& operator=(Optional&& move)
		{
			if (_isUsed)
			{
				_value.~ValueType();
			}

			_isUsed = move._isUsed;

			if (_isUsed)
			{
				new (&_value) ValueType(std::forward<ValueType>(move._value));
			}

			move._isUsed = false;

			return *this;
		}

		Optional& operator=(std::nullptr_t)
		{
			Clear();
			return *this;
		}

		void Clear()
		{
			if (_isUsed)
			{
				_value.~ValueType();
			}

			_isUsed = false;
		}

		bool IsValid() const
		{
			return _isUsed;
		}

		template<typename... Args>
		void Emplace(Args&&... args)
		{
			if (_isUsed)
			{
				_value.~ValueType();
			}

			new (&_value) ValueType(std::forward<Args>(args)...);

			_isUsed = true;
		}

		ValueType& Value()
		{
			return _value;
		}

		const ValueType& Value() const
		{
			return _value;
		}

		template<typename T>
		friend bool operator==(const Optional<T>& left, const Optional<T>& right);

		template<typename T>
		friend bool operator!=(const Optional<T>& left, const Optional<T>& right);

		template<typename T>
		friend bool operator<(const Optional<T>& left, const Optional<T>& right);

		template<typename T>
		friend bool operator>(const Optional<T>& left, const Optional<T>& right);

		template<typename T>
		friend bool operator<=(const Optional<T>& left, const Optional<T>& right);

		template<typename T>
		friend bool operator>=(const Optional<T>& left, const Optional<T>& right);

	private:
		// mlarouche - The union is used to disable the automatic call to a
		// complex type ctor/dtor. It allows defaut-constructed Optional of 
		// a complex type to be near no-op.
		union
		{
			std::uint8_t _dummy;
			ValueType _value;
		};
		bool _isUsed = false;
	};

	template<typename T>
	bool operator==(const Optional<T>& left, const Optional<T>& right)
	{
		if (left._isUsed == right._isUsed)
		{
			if (left._isUsed)
			{
				return left._value == right._value;
			}

			return true;
		}

		return false;
	}

	template<typename T>
	bool operator!=(const Optional<T>& left, const Optional<T>& right)
	{
		return !operator==(left, right);
	}

	template<typename T>
	bool operator<(const Optional<T>& left, const Optional<T>& right)
	{
		if (left._isUsed == right._isUsed)
		{
			if (left._isUsed)
			{
				return left._value < right._value;
			}

			return true;
		}

		return false;
	}

	template<typename T>
	bool operator>(const Optional<T>& left, const Optional<T>& right)
	{
		if (left._isUsed == right._isUsed)
		{
			if (left._isUsed)
			{
				return left._value > right._value;
			}

			return true;
		}

		return false;
	}

	template<typename T>
	bool operator<=(const Optional<T>& left, const Optional<T>& right)
	{
		if (left._isUsed == right._isUsed)
		{
			if (left._isUsed)
			{
				return left._value <= right._value;
			}

			return true;
		}

		return false;
	}

	template<typename T>
	bool operator>=(const Optional<T>& left, const Optional<T>& right)
	{
		if (left._isUsed == right._isUsed)
		{
			if (left._isUsed)
			{
				return left._value >= right._value;
			}

			return true;
		}

		return false;
	}
}