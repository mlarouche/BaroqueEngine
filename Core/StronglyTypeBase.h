#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	template<typename T, typename Parameter>
	struct StronglyTypeBase
	{
		T Value;
	};

	template<typename T, typename Parameter>
	inline bool operator==(const StronglyTypeBase<T, Parameter>& left, const StronglyTypeBase<T, Parameter>& right)
	{
		return left.Value == right.Value;
	}

	template<typename T, typename Parameter>
	inline bool operator!=(const StronglyTypeBase<T, Parameter>& left, const StronglyTypeBase<T, Parameter>& right)
	{
		return left.Value != right.Value;
	}

	template<typename T, typename Parameter>
	inline bool operator<(const StronglyTypeBase<T, Parameter>& left, const StronglyTypeBase<T, Parameter>& right)
	{
		return left.Value < right.Value;
	}

	template<typename T, typename Parameter>
	inline bool operator<=(const StronglyTypeBase<T, Parameter>& left, const StronglyTypeBase<T, Parameter>& right)
	{
		return left.Value <= right.Value;
	}

	template<typename T, typename Parameter>
	inline bool operator>(const StronglyTypeBase<T, Parameter>& left, const StronglyTypeBase<T, Parameter>& right)
	{
		return left.Value > right.Value;
	}

	template<typename T, typename Parameter>
	inline bool operator>=(const StronglyTypeBase<T, Parameter>& left, const StronglyTypeBase<T, Parameter>& right)
	{
		return left.Value >= right.Value;
	}

	template<typename T, typename Parameter>
	inline StronglyTypeBase<T, Parameter> operator!(const StronglyTypeBase<T, Parameter>& left)
	{
		return { !left.Value };
	}
}