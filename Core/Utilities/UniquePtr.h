#pragma once

#include "Core/CoreDefines.h"

#include "Core/Memory/Memory.h"
#include "Core/Utilities/TypeTraits.h"

namespace Baroque
{
	namespace Private
	{
		template<typename T>
		struct CppDeleter
		{
			using Pointer = typename std::remove_all_extents<T>::type*;

			constexpr void operator()(Pointer value)
			{
				delete value;
			}
		};

		template<typename T, typename Allocator>
		struct AllocatorDeleter
		{
			using Pointer = typename std::remove_all_extents<T>::type*;

			constexpr AllocatorDeleter(Allocator& allocator)
			: _allocator(allocator)
			{}

			constexpr void operator()(Pointer value)
			{
				_allocator.Deallocate(value);
			}

		private:
			Allocator& _allocator;
		};
	}

	template<typename T, typename Deleter = Private::CppDeleter<T>>
	class UniquePtr : private Deleter
	{
	public:
		using Pointer = typename std::remove_all_extents<T>::type*;
		using ConstPointer = typename std::remove_all_extents<T>::type const*;

		constexpr UniquePtr()
		{
		}

		constexpr UniquePtr(std::nullptr_t)
		: _ptr(nullptr)
		{
		}

		explicit constexpr UniquePtr(Pointer value)
		: _ptr(value)
		{
		}

		template<typename Allocator>
		explicit constexpr UniquePtr(Allocator& allocator, Pointer value)
		: Deleter(allocator)
		, _ptr(value)
		{
		}

		constexpr UniquePtr(const UniquePtr& copy) = delete;

		constexpr UniquePtr(UniquePtr&& move)
		: _ptr(move._ptr)
		{
			move._ptr = nullptr;
		}

		~UniquePtr()
		{
			destroy();
		}

		UniquePtr& operator=(std::nullptr_t)
		{
			Release();

			return *this;
		}

		UniquePtr& operator=(const UniquePtr& copy) = delete;

		UniquePtr& operator=(UniquePtr&& move)
		{
			destroy();

			_ptr = move._ptr;
			move._ptr = nullptr;

			return *this;
		}

		constexpr bool IsValid() const
		{
			return _ptr != nullptr;
		}

		constexpr explicit operator bool() const
		{
			return _ptr != nullptr;
		}

		constexpr ConstPointer Get() const
		{
			return _ptr;
		}

		constexpr Pointer Get()
		{
			return _ptr;
		}

		constexpr void Release()
		{
			destroy();

			_ptr = nullptr;
		}

		constexpr Pointer operator*()
		{
			return _ptr;
		}

		constexpr ConstPointer operator*() const
		{
			return _ptr;
		}

		constexpr Pointer operator->()
		{
			return _ptr;
		}

		constexpr ConstPointer operator->() const
		{
			return _ptr;
		}

	private:
		inline void destroy()
		{
			if (_ptr)
			{
				Deleter::operator()(_ptr);
			}
		}

	private:
		Pointer _ptr = nullptr;
	};

	template<typename T1, typename Deleter1, typename T2, typename Deleter2>
	inline bool operator==(const Baroque::UniquePtr<T1, Deleter1>& left, const Baroque::UniquePtr<T2, Deleter2>& right)
	{
		return left.Get() == right.Get();
	}

	template<typename T1, typename Deleter1, typename T2, typename Deleter2>
	inline bool operator!=(const Baroque::UniquePtr<T1, Deleter1>& left, const Baroque::UniquePtr<T2, Deleter2>& right)
	{
		return left.Get() != right.Get();
	}

	template<typename T1, typename Deleter1, typename T2, typename Deleter2>
	inline bool operator<(const Baroque::UniquePtr<T1, Deleter1>& left, const Baroque::UniquePtr<T2, Deleter2>& right)
	{
		return left.Get() < right.Get();
	}

	template<typename T1, typename Deleter1, typename T2, typename Deleter2>
	inline bool operator<=(const Baroque::UniquePtr<T1, Deleter1>& left, const Baroque::UniquePtr<T2, Deleter2>& right)
	{
		return left.Get() <= right.Get();
	}

	template<typename T1, typename Deleter1, typename T2, typename Deleter2>
	inline bool operator>(const Baroque::UniquePtr<T1, Deleter1>& left, const Baroque::UniquePtr<T2, Deleter2>& right)
	{
		return left.Get() > right.Get();
	}

	template<typename T1, typename Deleter1, typename T2, typename Deleter2>
	inline bool operator>=(const Baroque::UniquePtr<T1, Deleter1>& left, const Baroque::UniquePtr<T2, Deleter2>& right)
	{
		return left.Get() >= right.Get();
	}

	template<typename T, typename Deleter>
	inline bool operator==(const Baroque::UniquePtr<T, Deleter>& left, std::nullptr_t)
	{
		return left.Get() == nullptr;
	}

	template<typename T, typename Deleter>
	inline bool operator!=(const Baroque::UniquePtr<T, Deleter>& left, std::nullptr_t)
	{
		return left.Get() != nullptr;
	}

	template<typename T, typename Deleter>
	inline bool operator<(const Baroque::UniquePtr<T, Deleter>& left, std::nullptr_t)
	{
		return left.Get() < nullptr;
	}

	template<typename T, typename Deleter>
	inline bool operator<=(const Baroque::UniquePtr<T, Deleter>& left, std::nullptr_t)
	{
		return left.Get() <= nullptr;
	}

	template<typename T, typename Deleter>
	inline bool operator>(const Baroque::UniquePtr<T, Deleter>& left, std::nullptr_t)
	{
		return left.Get() > nullptr;
	}

	template<typename T, typename Deleter>
	inline bool operator>=(const Baroque::UniquePtr<T, Deleter>& left, std::nullptr_t)
	{
		return left.Get() >= nullptr;
	}

	template<typename T, typename Deleter>
	inline bool operator==(std::nullptr_t, const Baroque::UniquePtr<T, Deleter>& right)
	{
		return nullptr == right.Get();
	}

	template<typename T, typename Deleter>
	inline bool operator!=(std::nullptr_t, const Baroque::UniquePtr<T, Deleter>& right)
	{
		return nullptr != right.Get();
	}

	template<typename T, typename Deleter>
	inline bool operator<(std::nullptr_t, const Baroque::UniquePtr<T, Deleter>& right)
	{
		return nullptr < right.Get();
	}

	template<typename T, typename Deleter>
	inline bool operator<=(std::nullptr_t, const Baroque::UniquePtr<T, Deleter>& right)
	{
		return nullptr <= right.Get();
	}

	template<typename T, typename Deleter>
	inline bool operator>(std::nullptr_t, const Baroque::UniquePtr<T, Deleter>& right)
	{
		return nullptr > right.Get();
	}

	template<typename T, typename Deleter>
	inline bool operator>=(std::nullptr_t, const Baroque::UniquePtr<T, Deleter>& right)
	{
		return nullptr >= right.Get();
	}

	template<typename T, typename... Args>
	constexpr Baroque::UniquePtr<T> MakeUnique(Args&&... args)
	{
		return Baroque::UniquePtr<T>{new T{ std::forward<Args>(args)... }};
	}

	template<typename T, typename ObjectAllocator, typename... Args>
	constexpr Baroque::UniquePtr<T, Private::AllocatorDeleter<T, ObjectAllocator>> MakeUniqueAllocator(ObjectAllocator& allocator, Args&& ... args)
	{
		return Baroque::UniquePtr<T, Private::AllocatorDeleter<T, ObjectAllocator>>(allocator, allocator.Allocate(std::forward<Args>(args)...));
	}
}