#pragma once

#include "Core/CoreDefines.h"

#include "Core/Unicode/Codepoint.h"

namespace Baroque
{
	namespace Unicode
	{
		// TODO: Use SIMD-optimized version
		constexpr std::size_t ByteLength(const char* ptr)
		{
			if (!ptr)
			{
				return 0;
			}

			std::size_t size = 0;

			while (*ptr != '\0')
			{
				++size;
				++ptr;
			}

			return size;
		}

		// TODO: Try to optimize it with SIMD
		constexpr std::size_t CodepointLength(const char* ptr)
		{
			std::size_t size = 0;

			while (*ptr != '\0')
			{
				++size;

				ptr += Unicode::CodepointJumpTable[static_cast<std::uint8_t>(*ptr) >> 3];
			}

			return size;
		}
	}
}