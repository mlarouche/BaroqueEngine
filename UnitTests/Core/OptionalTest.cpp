#include <gtest/gtest.h>

#include "Core/Optional.h"

#include "TestComplexType.h"

TEST(Optional, ShouldBeNullWithEmptyConstructor)
{
	Baroque::Optional<int> null;

	EXPECT_FALSE(null.IsValid());
}

TEST(Optional, ShouldBeAbleToConstructWithSimpleType)
{
	Baroque::Optional<int> optional(34);

	EXPECT_EQ(optional.Value(), 34);
}

TEST(Optional, ShouldNotCallTypeCtorDtorWithNullValue)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> null;
	}

	EXPECT_EQ(TestComplexType::CtorCount, 0);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 0);
}

TEST(Optional, ShouldCallTypeCtorAndDtorAtCtorTime)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> test{ TestComplexType{} };

		EXPECT_EQ(test.Value().Value, 42);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 1);
	EXPECT_EQ(TestComplexType::DtorCount, 1);
}

TEST(Optional, ShouldBeAbleToDoImplaceCtor)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> test{Baroque::InPlace, 128};

		EXPECT_EQ(test.Value().Value, 128);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 1);
}

TEST(Optional, CopyConstructorShouldWorkWithSimpleType)
{
	Baroque::Optional<int> valid(34);
	Baroque::Optional<int> copy(valid);

	EXPECT_TRUE(copy.IsValid());
	EXPECT_EQ(copy.Value(), 34);
}

TEST(Optional, CopyConstructorShouldCallCopyCtorOfComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> valid(Baroque::InPlace, 128);
		Baroque::Optional<TestComplexType> copy(valid);

		EXPECT_TRUE(copy.IsValid());
		EXPECT_EQ(copy.Value().Value, 128);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 1);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 2);
}

TEST(Optional, MoveConstructorShouldOnlyCallDtorOnce)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> valid(std::move(Baroque::Optional<TestComplexType>{Baroque::InPlace, 567}));

		EXPECT_TRUE(valid.IsValid());
		EXPECT_EQ(valid.Value().Value, 567);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 1);
	EXPECT_EQ(TestComplexType::DtorCount, 1);
}

TEST(Optional, AssignmentWithNullptrShouldClearTheOptionalValue)
{
	Baroque::Optional<int> testNullptr{ 1234 };

	EXPECT_TRUE(testNullptr.IsValid());

	testNullptr = nullptr;

	EXPECT_FALSE(testNullptr.IsValid());
}

TEST(Optional, AssignmentWithNullptrShouldCallTheDestructor)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> testNullptr{ Baroque::InPlace, 1234 };

		EXPECT_TRUE(testNullptr.IsValid());

		testNullptr = nullptr;

		EXPECT_FALSE(testNullptr.IsValid());
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 1);
}

TEST(Optional, ClearShouldResetTheOptionalValue)
{
	Baroque::Optional<int> testNullptr{ 1234 };

	EXPECT_TRUE(testNullptr.IsValid());

	testNullptr.Clear();

	EXPECT_FALSE(testNullptr.IsValid());
}

TEST(Optional, ClearShouldCallTheDestructor)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> testNullptr{ Baroque::InPlace, 1234 };

		EXPECT_TRUE(testNullptr.IsValid());

		testNullptr.Clear();

		EXPECT_FALSE(testNullptr.IsValid());
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 1);
}

TEST(Optional, CopyAssignmentOperatorShouldWorkWithSimpleType)
{
	Baroque::Optional<int> first{ 12 };
	Baroque::Optional<int> second{ 24 };

	EXPECT_EQ(first.Value(), 12);
	EXPECT_EQ(second.Value(), 24);

	first = second;

	EXPECT_TRUE(first.IsValid());
	EXPECT_EQ(first.Value(), 24);

	second = 678;

	EXPECT_TRUE(second.IsValid());
	EXPECT_EQ(second.Value(), 678);
}

TEST(Optional, CopyAssignmentOperatorShouldWorkWithComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> first{ Baroque::InPlace, 34 };
		Baroque::Optional<TestComplexType> second{ Baroque::InPlace, 90 };

		EXPECT_EQ(first.Value().Value, 34);
		EXPECT_EQ(second.Value().Value, 90);

		first = second;

		EXPECT_TRUE(first.IsValid());
		EXPECT_EQ(first.Value().Value, 90);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 1);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 3);
}

TEST(Optional, DirectCopyAssignmentOperatorShouldWorkWithComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> optional{ Baroque::InPlace, 34 };
		EXPECT_EQ(optional.Value().Value, 34);

		TestComplexType copy{ 567 };
		optional = copy;

		EXPECT_EQ(optional.Value().Value, 567);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 1);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 3);
}

TEST(Optional, DirectCopyAssignmentOperatorShouldSetIsValid)
{
	Baroque::Optional<int> test;

	EXPECT_FALSE(test.IsValid());

	test = 456;

	EXPECT_TRUE(test.IsValid());
	EXPECT_EQ(test.Value(), 456);
}

TEST(Optional, CopyAssignANullValueShouldCallDestructorOfComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> first{ Baroque::InPlace, 34 };
		Baroque::Optional<TestComplexType> null;

		EXPECT_EQ(first.Value().Value, 34);
		EXPECT_FALSE(null.IsValid());

		first = null;

		EXPECT_FALSE(first.IsValid());
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 1);
}

TEST(Optional, MoveAssignmentOperatorShouldWorkWithComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> first{ Baroque::InPlace, 34 };
		Baroque::Optional<TestComplexType> second{ Baroque::InPlace, 90 };

		EXPECT_EQ(first.Value().Value, 34);
		EXPECT_EQ(second.Value().Value, 90);

		first = std::move(second);

		EXPECT_TRUE(first.IsValid());
		EXPECT_EQ(first.Value().Value, 90);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 1);
	EXPECT_EQ(TestComplexType::DtorCount, 2);
}

TEST(Optional, DirectMoveAssigmentShouldWorkWithComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> testMoveDirect;

		EXPECT_FALSE(testMoveDirect.IsValid());

		testMoveDirect = TestComplexType{ 987 };

		EXPECT_TRUE(testMoveDirect.IsValid());
		EXPECT_EQ(testMoveDirect.Value().Value, 987);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 1);
	EXPECT_EQ(TestComplexType::DtorCount, 1);
}

TEST(Optional, DirectMoveAssigmentShouldCallTheDtorOfComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> testMoveDirect{ Baroque::InPlace, 123 };
		EXPECT_TRUE(testMoveDirect.IsValid());
		EXPECT_EQ(testMoveDirect.Value().Value, 123);

		testMoveDirect = TestComplexType{ 987 };

		EXPECT_TRUE(testMoveDirect.IsValid());
		EXPECT_EQ(testMoveDirect.Value().Value, 987);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 1);
	EXPECT_EQ(TestComplexType::DtorCount, 2);
}

TEST(Optional, EmplaceShouldWork)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> testEmplace;

		EXPECT_FALSE(testEmplace.IsValid());

		testEmplace.Emplace(345);

		EXPECT_TRUE(testEmplace.IsValid());

		EXPECT_EQ(testEmplace.Value().Value, 345);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 1);
}

TEST(Optional, EmplaceShouldCallDtorWhenHavingAlreadyAValidValue)
{
	TestComplexType::Reset();

	{
		Baroque::Optional<TestComplexType> testEmplace{ Baroque::InPlace, 123 };

		EXPECT_TRUE(testEmplace.IsValid());

		testEmplace.Emplace(345);

		EXPECT_TRUE(testEmplace.IsValid());

		EXPECT_EQ(testEmplace.Value().Value, 345);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 2);
}

TEST(Optional, OperatorEqualsShouldWork)
{
	Baroque::Optional<int> firstNull;
	Baroque::Optional<int> secondNull;

	EXPECT_EQ(firstNull, secondNull);

	Baroque::Optional<int> firstValid{ 12 };
	Baroque::Optional<int> secondValid{ 12 };
	Baroque::Optional<int> differentValid{ 34 };

	EXPECT_EQ(firstValid, secondValid);
	EXPECT_FALSE(firstValid == firstNull);
	EXPECT_FALSE(firstValid == differentValid);
}

TEST(Optional, OperatorNotEqualsShouldWork)
{
	Baroque::Optional<int> firstNull;
	Baroque::Optional<int> firstValid{ 12 };
	Baroque::Optional<int> secondValid{ 12 };

	EXPECT_NE(firstNull, firstValid);
	EXPECT_FALSE(firstValid != secondValid);
}

TEST(Optional, OperatorLessShouldWork)
{
	Baroque::Optional<int> firstNull;
	Baroque::Optional<int> secondNull;

	Baroque::Optional<int> valid{ 12 };
	Baroque::Optional<int> differentValid{ 34 };

	EXPECT_LT(firstNull, secondNull);
	EXPECT_LT(valid, differentValid);

	EXPECT_FALSE(differentValid < valid);
}

TEST(Optional, OperatorGreaterShouldWork)
{
	Baroque::Optional<int> firstNull;
	Baroque::Optional<int> secondNull;

	Baroque::Optional<int> valid{ 12 };
	Baroque::Optional<int> differentValid{ 34 };

	EXPECT_GT(firstNull, secondNull);
	EXPECT_GT(differentValid, valid);

	EXPECT_FALSE(valid > differentValid);
}

TEST(Optional, OperatorLessEqualsShouldWork)
{
	Baroque::Optional<int> firstNull;
	Baroque::Optional<int> secondNull;

	Baroque::Optional<int> valid{ 12 };
	Baroque::Optional<int> secondValid{ 12 };
	Baroque::Optional<int> differentValid{ 34 };

	EXPECT_LE(firstNull, secondNull);
	EXPECT_LE(valid, differentValid);
	EXPECT_LE(valid, secondValid);

	EXPECT_FALSE(differentValid <= valid);
}

TEST(Optional, OperatorGreaterEqualsShouldWork)
{
	Baroque::Optional<int> firstNull;
	Baroque::Optional<int> secondNull;

	Baroque::Optional<int> valid{ 12 };
	Baroque::Optional<int> differentValid{ 34 };
	Baroque::Optional<int> differentSecondValid{ 34 };

	EXPECT_GE(firstNull, secondNull);
	EXPECT_GE(differentValid, valid);
	EXPECT_GE(differentSecondValid, differentValid);

	EXPECT_FALSE(valid >= differentValid);
}