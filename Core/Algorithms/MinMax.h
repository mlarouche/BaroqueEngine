#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	namespace Algorithm
	{
		template<typename T>
		constexpr T Min(T left, T right)
		{
			return left < right ? left : right;
		}

		template<typename T>
		constexpr T Max(T left, T right)
		{
			return left > right ? left : right;
		}
	}
}