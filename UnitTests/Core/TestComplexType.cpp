#include "TestComplexType.h"

std::size_t TestComplexType::CtorCount = 0;
std::size_t TestComplexType::CopyCtorCount = 0;
std::size_t TestComplexType::MoveCtorCount = 0;
std::size_t TestComplexType::DtorCount = 0;
std::size_t TestComplexType::CopyAssignmentCount = 0;

TestComplexType::TestComplexType()
{
	++CtorCount;
}

TestComplexType::TestComplexType(int Value)
: TestComplexType()
{
	DummyValue = Value;
}

TestComplexType::~TestComplexType()
{
	if (DoDtor)
	{
		++DtorCount;
	}
}

TestComplexType::TestComplexType(const TestComplexType& copy)
: DummyValue(copy.DummyValue)
{
	++CopyCtorCount;
}

TestComplexType::TestComplexType(TestComplexType&& move)
: DummyValue(move.DummyValue)
{
	++MoveCtorCount;
	move.DoDtor = false;
}

TestComplexType& TestComplexType::operator=(const TestComplexType& copy)
{
	if (this != &copy)
	{
		DummyValue = copy.DummyValue;
		++CopyAssignmentCount;
	}

	return *this;
}

void TestComplexType::Reset()
{
	CtorCount = 0;
	CopyCtorCount = 0;
	MoveCtorCount = 0;
	DtorCount = 0;
	CopyAssignmentCount = 0;
}
