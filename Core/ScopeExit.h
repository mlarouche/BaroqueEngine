#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	template<typename Function>
	class ScopeExit
	{
	public:
		constexpr ScopeExit(Function&& function)
		: _function(function)
		{}

		~ScopeExit()
		{
			_function();
		}

	private:
		Function _function;
	};

	template<typename Function>
	constexpr Baroque::ScopeExit<Function> MakeScopeExit(Function&& function)
	{
		return Baroque::ScopeExit<Function>{std::forward<Function>(function)};
	}

#define BAROQUE_SCOPE_EXIT(Function) auto BAROQUE_UNIQUE_VARIABLE(__baroque_scope_exit) = Baroque::MakeScopeExit(Function)
}