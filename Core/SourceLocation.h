#pragma once

#include "CoreDefines.h"

namespace Baroque
{
	// TODO: mlarouche Implement static Current() function when all compilers supports proper faciliy
	// to retrieve the filename and line
	// Similar to C++20 std::source_location
	struct SourceLocation
	{
		constexpr SourceLocation() = default;

		constexpr SourceLocation(const char* filename, const char* function, std::uint32_t line)
		: FileName(filename)
		, Function(function)
		, Line(line)
		{
		}

		const char* FileName = nullptr;
		const char* Function = nullptr;
		std::uint32_t Line = 0;
	};
}

#if defined(BAROQUE_COMPILER_MSVC)
#define BAROQUE_SOURCE_LOCATION Baroque::SourceLocation{__FILE__, __func__ , static_cast<std::uint32_t>(__LINE__)}
#else
#error "Define Baroque::SourceLocation for your compiler"
#endif