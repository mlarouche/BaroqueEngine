#pragma once

#include "Core/CoreDefines.h"

#include <type_traits>

namespace Baroque
{
	namespace Traits
	{
		template<typename T>
		struct IsTriviallyRelocatable : public std::bool_constant<std::is_trivially_move_constructible_v<T>>
		{
		};

		template <class T>
		inline constexpr bool IsTriviallyRelocatable_v = IsTriviallyRelocatable<T>::value;
	}
}

#define BAROQUE_TRIVIALLY_RELOCABLE(type) \
		template<> \
		struct IsTriviallyRelocatable<type> : public std::bool_constant<true> \
		{}