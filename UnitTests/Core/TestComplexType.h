#pragma once

#include "Core/CoreDefines.h"

struct TestComplexType
{
	TestComplexType();
	TestComplexType(int Value);
	~TestComplexType();
	TestComplexType(const TestComplexType& copy);
	TestComplexType(TestComplexType&& move);

	TestComplexType& operator=(const TestComplexType& copy);

	int DummyValue = 42;
	bool DoDtor = true;

	static void Reset();

	static std::size_t CtorCount;
	static std::size_t CopyCtorCount;
	static std::size_t MoveCtorCount;
	static std::size_t DtorCount;
	static std::size_t CopyAssignmentCount;
};