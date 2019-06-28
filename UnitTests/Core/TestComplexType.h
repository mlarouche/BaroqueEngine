#pragma once

#include "Core/CoreDefines.h"

struct TestComplexType
{
	TestComplexType();
	TestComplexType(int value);
	~TestComplexType();
	TestComplexType(const TestComplexType& copy);
	TestComplexType(TestComplexType&& move);

	TestComplexType& operator=(const TestComplexType& copy);

	int Value = 42;
	bool DoDtor = true;

	static void Reset();

	static std::size_t CtorCount;
	static std::size_t CopyCtorCount;
	static std::size_t MoveCtorCount;
	static std::size_t DtorCount;
	static std::size_t CopyAssignmentCount;
};

bool operator==(const TestComplexType& left, const TestComplexType& right);