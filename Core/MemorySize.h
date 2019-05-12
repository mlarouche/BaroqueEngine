#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	template<std::size_t SizeFactor>
	struct MemorySize
	{
	public:
		constexpr MemorySize(const std::size_t size)
			: _actualSize(size * SizeFactor)
		{}

		constexpr operator std::size_t() const
		{
			return _actualSize;
		}

	private:
		std::size_t _actualSize;
	};

	using KB = MemorySize<1024>;
	using MB = MemorySize<1024 * 1024>;
	using GB = MemorySize<1024 * 1024 * 1024>;

	constexpr KB operator""_KB(const std::size_t size)
	{
		return KB{ size };
	}

	constexpr MB operator""_MB(const std::size_t size)
	{
		return MB{ size };
	}

	constexpr GB operator""_GB(const std::size_t size)
	{
		return GB{ size };
	}
}