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

TestComplexType::TestComplexType(int value)
: TestComplexType()
{
	Value = value;
}

TestComplexType::~TestComplexType()
{
	if (DoDtor)
	{
		++DtorCount;
	}
}

TestComplexType::TestComplexType(const TestComplexType& copy)
: Value(copy.Value)
{
	++CopyCtorCount;
}

TestComplexType::TestComplexType(TestComplexType&& move)
: Value(move.Value)
{
	++MoveCtorCount;
	move.DoDtor = false;
}

TestComplexType& TestComplexType::operator=(const TestComplexType& copy)
{
	if (this != &copy)
	{
		Value = copy.Value;
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

bool operator==(const TestComplexType& left, const TestComplexType& right)
{
	return left.DoDtor == right.DoDtor
		&& left.Value == right.Value
		;
}
