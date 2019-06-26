#include <gtest/gtest.h>

#include "Core/UniquePtr.h"
#include "TestComplexType.h"

#include "Core/Memory/ObjectAllocator.h"
#include "Core/Memory/Memory.h"

namespace
{
	struct TestCustomDeleterNoState
	{
		void operator()(TestComplexType* value)
		{
			delete value;
			++CustomDtor;
		}

		static void Reset()
		{
			CustomDtor = 0;
		}

		static std::size_t CustomDtor;
	};

	std::size_t TestCustomDeleterNoState::CustomDtor = 0;
}

TEST(UniquePtr, UniquePtrWithNewDeleteWithSimpleType)
{
	Baroque::UniquePtr<int> simple = Baroque::MakeUnique<int>();
	EXPECT_TRUE(simple.IsValid());

	*simple.Get() = 42;

	EXPECT_EQ(*simple.Get(), 42);

	simple.Release();

	EXPECT_FALSE(simple.IsValid());

	EXPECT_EQ(sizeof(simple), sizeof(int*));
}

TEST(UniquePtr, OperatorBoolShouldWork)
{
	Baroque::UniquePtr<int> null;
	EXPECT_FALSE(null);

	Baroque::UniquePtr<int> valid = Baroque::MakeUnique<int>(34);
	EXPECT_TRUE(valid);
}

TEST(UniquePtr, ShouldCallDestructorOfComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::UniquePtr<TestComplexType> complex = Baroque::MakeUnique<TestComplexType>(42);

		EXPECT_EQ(complex->DummyValue, 42);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 1);
}

TEST(UniquePtr, ShouldCallProperlyTheCustomDeleter)
{
	TestComplexType::Reset();
	TestCustomDeleterNoState::Reset();

	{
		Baroque::UniquePtr<TestComplexType, TestCustomDeleterNoState> customDeleter(new TestComplexType);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 1);

	EXPECT_EQ(TestCustomDeleterNoState::CustomDtor, 1);
}

TEST(UniquePtr, ShouldNotCallCustomDeleterOnInvalidUniquePtr)
{
	TestCustomDeleterNoState::Reset();

	{
		Baroque::UniquePtr<TestComplexType, TestCustomDeleterNoState> customDeleter;
	}

	EXPECT_EQ(TestCustomDeleterNoState::CustomDtor, 0);
}

TEST(UniquePtr, ShouldOnlyCallCustomDeleterOnceWhenReleasing)
{
	TestCustomDeleterNoState::Reset();

	{
		Baroque::UniquePtr<TestComplexType, TestCustomDeleterNoState> customDeleter(new TestComplexType);
		EXPECT_TRUE(customDeleter.IsValid());

		customDeleter.Release();

		EXPECT_FALSE(customDeleter.IsValid());
	}

	EXPECT_EQ(TestCustomDeleterNoState::CustomDtor, 1);
}

TEST(UniquePtr, MoveConstructor)
{
	Baroque::UniquePtr<int> original(new int(42));
	Baroque::UniquePtr<int> moved(std::move(original));

	EXPECT_TRUE(moved);
	EXPECT_FALSE(original);
}

TEST(UniquePtr, MoveAssignmentShouldWorkWithSimpleDeleter)
{
	Baroque::UniquePtr<int> original(new int(42));
	Baroque::UniquePtr<int> moved = std::move(original);

	EXPECT_TRUE(moved);
	EXPECT_FALSE(original);
}

TEST(UniquePtr, MoveAssignmentShouldWorkWithCustomDeleter)
{
	TestCustomDeleterNoState::Reset();

	{
		Baroque::UniquePtr<TestComplexType, TestCustomDeleterNoState> original(new TestComplexType{ 42 });
		Baroque::UniquePtr<TestComplexType, TestCustomDeleterNoState> moved(new TestComplexType{ 123 });

		moved = std::move(original);
		EXPECT_TRUE(moved);
		EXPECT_FALSE(original);

		EXPECT_EQ(moved->DummyValue, 42);
	}

	EXPECT_EQ(TestCustomDeleterNoState::CustomDtor, 2);
}

TEST(UniquePtr, AssignmentWithNullPointerShouldCallDestructor)
{
	TestCustomDeleterNoState::Reset();

	{
		Baroque::UniquePtr<TestComplexType, TestCustomDeleterNoState> valid(new TestComplexType{ 42 });
		EXPECT_EQ(valid->DummyValue, 42);

		valid = nullptr;

		EXPECT_FALSE(valid);
	}

	EXPECT_EQ(TestCustomDeleterNoState::CustomDtor, 1);
}

TEST(UniquePtr, OperatorEquals)
{
	Baroque::UniquePtr<int> firstNull;
	Baroque::UniquePtr<int> secondNull;

	EXPECT_EQ(firstNull, secondNull);

	EXPECT_EQ(firstNull, nullptr);
}

TEST(UniquePtr, OperatorNotEquals)
{
	Baroque::UniquePtr<int> null;
	Baroque::UniquePtr<int> valid = Baroque::MakeUnique<int>(12);

	EXPECT_NE(null, valid);

	EXPECT_NE(valid, nullptr);
}

TEST(UniquePtr, OperatorLess)
{
	Baroque::UniquePtr<int> null;
	Baroque::UniquePtr<int> valid = Baroque::MakeUnique<int>(12);

	EXPECT_LT(null, valid);

	EXPECT_LT(nullptr, valid);
}

TEST(UniquePtr, OperatorLessEquals)
{
	Baroque::UniquePtr<int> null;
	Baroque::UniquePtr<int> valid = Baroque::MakeUnique<int>(12);

	EXPECT_LE(null, valid);

	EXPECT_LE(nullptr, valid);
}

TEST(UniquePtr, OperatorGreater)
{
	Baroque::UniquePtr<int> null;
	Baroque::UniquePtr<int> valid = Baroque::MakeUnique<int>(12);

	EXPECT_GT(valid, null);

	EXPECT_GT(valid, nullptr);
}

TEST(UniquePtr, OperatorGreaterOrEqual)
{
	Baroque::UniquePtr<int> null;
	Baroque::UniquePtr<int> valid = Baroque::MakeUnique<int>(12);

	EXPECT_GE(valid, null);

	EXPECT_GE(valid, nullptr);
}

TEST(UniquePtr, MakeUniqueWithAllocator)
{
	Baroque::Memory::ObjectAllocator<int, Baroque::Memory::StackAllocator<512>> stackIntAlloc;

	auto stackUniquePtr = Baroque::MakeUniqueAllocator<int>(stackIntAlloc, 33);

	EXPECT_TRUE(stackUniquePtr);
	EXPECT_EQ(*stackUniquePtr.Get(), 33);
}
