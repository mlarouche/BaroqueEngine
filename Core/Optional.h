#pragma once

#include "Core/CoreDefines.h"

#include "Core/TypeTraits.h"

namespace Baroque
{
	struct InPlaceType {};
	inline constexpr InPlaceType InPlace{};

	namespace Private
	{
		template<typename T, bool isTrivialDestructible>
		class OptionalStorage
		{
		protected:
			using ValueType = T;

			OptionalStorage()
			: _dummy('\0')
			{
			}

			OptionalStorage(const ValueType& value)
			: _isUsed(true)
			, _value(value)
			{}

			OptionalStorage(ValueType&& value)
			: _isUsed(true)
			, _value(std::forward<T>(value))
			{}

			OptionalStorage(const OptionalStorage& copy)
			: _isUsed(copy._isUsed)
			{
				if (_isUsed)
				{
					new (&_value) ValueType(copy._value);
				}
			}

			OptionalStorage(OptionalStorage&& move)
			: _isUsed(move._isUsed)
			{
				move._isUsed = false;

				if (_isUsed)
				{
					new (&_value) ValueType(std::forward<ValueType>(move._value));
				}
			}

			template<typename... Args>
			OptionalStorage(InPlaceType, Args&& ... args)
			: _isUsed(true)
			{
				new (&_value) ValueType(std::forward<Args>(args)...);
			}

			~OptionalStorage()
			{
				destroy();
			}

			void destroy()
			{
				if (_isUsed)
				{
					_value.~ValueType();
				}
			}

		protected:
			// mlarouche - The union is used to disable the automatic call to a
			// complex type ctor/dtor. It allows defaut-constructed Optional of 
			// a complex type to be near no-op.
			union
			{
				std::uint8_t _dummy;
				T _value;
			};

			bool _isUsed = false;
		};

		// Trivial case
		template<typename T>
		class OptionalStorage<T, true>
		{
		protected:
			using ValueType = T;

			OptionalStorage()
			: _dummy('\0')
			{
			}

			OptionalStorage(const ValueType& value)
			: _isUsed(true)
			, _value(value)
			{}

			OptionalStorage(ValueType&& value)
			: _isUsed(true)
			, _value(std::forward<T>(value))
			{}

			OptionalStorage(const OptionalStorage& copy)
			: _isUsed(copy._isUsed)
			{
				if (_isUsed)
				{
					new (&_value) ValueType(copy._value);
				}
			}

			OptionalStorage(OptionalStorage&& move)
				: _isUsed(move._isUsed)
			{
				move._isUsed = false;

				if (_isUsed)
				{
					new (&_value) ValueType(std::forward<ValueType>(move._value));
				}
			}

			template<typename... Args>
			OptionalStorage(InPlaceType, Args&& ... args)
			: _isUsed(true)
			{
				new (&_value) ValueType(std::forward<Args>(args)...);
			}

			~OptionalStorage()
			{
			}

			void destroy()
			{
			}

		protected:
			// mlarouche - The union is used to disable the automatic call to a
			// complex type ctor/dtor. It allows defaut-constructed Optional of 
			// a complex type to be near no-op.
			union
			{
				std::uint8_t _dummy;
				T _value;
			};

			bool _isUsed = false;
		};
	}

	template<typename T>
	class Optional : private Private::OptionalStorage<T, std::is_trivially_destructible<T>::value>
	{
	public:
		using ValueType = T;
		using Base = Private::OptionalStorage<T, std::is_trivially_destructible<T>::value>;

		Optional()
		: Base()
		{
		}

		Optional(const ValueType& value)
		: Base(value)
		{
		}

		Optional(ValueType&& value)
		: Base(std::forward<ValueType>(value))
		{
		}

		template<typename... Args>
		Optional(InPlaceType, Args&& ... args)
		: Base(InPlace, std::forward<Args>(args)...)
		{
		}

		Optional(const Optional& copy)
		: Base(copy)
		{
		}

		Optional(Optional&& move)
		: Base(std::forward<Base>(move))
		{
		}

		~Optional()
		{
		}

		Optional& operator=(const Optional& copy)
		{
			if (this != &copy)
			{
				this->destroy();

				this->_isUsed = copy._isUsed;

				if (this->_isUsed)
				{
					new (&this->_value) ValueType(copy._value);
				}
			}

			return *this;
		}

		Optional& operator=(Optional&& move)
		{
			this->destroy();

			this->_isUsed = move._isUsed;

			if (this->_isUsed)
			{
				new (&this->_value) ValueType(std::forward<ValueType>(move._value));
			}

			move._isUsed = false;

			return *this;
		}

		Optional& operator=(const ValueType& value)
		{
			this->destroy();

			new (&this->_value) ValueType(value);
			this->_isUsed = true;

			return *this;
		}

		Optional& operator=(ValueType&& value)
		{
			this->destroy();

			new (&this->_value) ValueType(std::forward<ValueType>(value));
			this->_isUsed = true;

			return *this;
		}

		Optional& operator=(std::nullptr_t)
		{
			Clear();
			return *this;
		}

		void Clear()
		{
			this->destroy();

			this->_isUsed = false;
		}

		bool IsValid() const
		{
			return this->_isUsed;
		}

		template<typename... Args>
		void Emplace(Args&&... args)
		{
			this->destroy();

			new (&this->_value) ValueType(std::forward<Args>(args)...);

			this->_isUsed = true;
		}

		ValueType& Value()
		{
			return this->_value;
		}

		const ValueType& Value() const
		{
			return this->_value;
		}
	};

	template<typename T>
	inline bool operator==(const Optional<T>& left, const Optional<T>& right)
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
	inline bool operator!=(const Optional<T>& left, const Optional<T>& right)
	{
		return !operator==(left, right);
	}

	template<typename T>
	inline bool operator<(const Optional<T>& left, const Optional<T>& right)
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
	inline bool operator>(const Optional<T>& left, const Optional<T>& right)
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
	inline bool operator<=(const Optional<T>& left, const Optional<T>& right)
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
	inline bool operator>=(const Optional<T>& left, const Optional<T>& right)
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