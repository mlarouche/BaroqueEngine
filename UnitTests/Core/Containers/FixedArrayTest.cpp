#include <gtest/gtest.h>

#include "Core/Containers/FixedArray.h"

#include "UnitTests/Core/TestComplexType.h"

namespace
{
	struct TrivialComplexType
	{
		const char* String;
		int Integer;
	};

	constexpr const auto TestSize = 8;
}

TEST(FixedArray, ShouldInitToZeroArrayOfSimpleTypes)
{
	Baroque::FixedArray<bool, TestSize> boolArray;
	Baroque::FixedArray<int, TestSize> intArray;
	Baroque::FixedArray<float, TestSize> floatArray;
	Baroque::FixedArray<double, TestSize> doubleArray;
	Baroque::FixedArray<TrivialComplexType, TestSize> trivialArray;

	for (std::size_t i = 0; i < TestSize; ++i)
	{
		EXPECT_FALSE(boolArray[i]);
		EXPECT_EQ(intArray[i], 0);
		EXPECT_EQ(floatArray[i], 0.f);
		EXPECT_EQ(doubleArray[i], 0.0);

		EXPECT_EQ(trivialArray[i].Integer, 0);
		EXPECT_EQ(trivialArray[i].String, nullptr);
	}
}

TEST(FixedArray, ShouldCallCtorAndDtorOfComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::FixedArray<TestComplexType, TestSize> complexArray;
	}

	EXPECT_EQ(TestComplexType::CtorCount, TestSize);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, TestSize);
}

TEST(FixedArray, ShouldCopyArrayOfSimpleTypes)
{
	Baroque::FixedArray<int, TestSize> original;

	for (std::size_t i = 0; i < TestSize; ++i)
	{
		original[i] = static_cast<int>(i + 1);
	}

	Baroque::FixedArray<int, TestSize> copy(original);

	for (int i = 0; i < TestSize; ++i)
	{
		EXPECT_EQ(copy[i], i + 1);
	}
}

TEST(FixedArray, ShouldCopyArrayOfComplexTypesWithCopyCtor)
{
	TestComplexType::Reset();

	{
		Baroque::FixedArray<TestComplexType, TestSize> original;

		for (std::size_t i = 0; i < TestSize; ++i)
		{
			original[i].Value = static_cast<int>(i + 1);
		}

		Baroque::FixedArray<TestComplexType, TestSize> copy(original);

		for (int i = 0; i < TestSize; ++i)
		{
			EXPECT_EQ(copy[i].Value, i + 1);
		}
	}

	EXPECT_EQ(TestComplexType::CtorCount, TestSize * 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, TestSize);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, TestSize * 2);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(FixedArray, ShouldMoveArrayOfSimpleTypes)
{
	Baroque::FixedArray<int, TestSize> original;

	for (std::size_t i = 0; i < TestSize; ++i)
	{
		original[i] = static_cast<int>(i + 1);
	}

	Baroque::FixedArray<int, TestSize> moved(std::move(original));

	for (int i = 0; i < TestSize; ++i)
	{
		EXPECT_EQ(moved[i], i + 1);
	}
}

TEST(FixedArray, ShouldMoveArrayOfComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::FixedArray<TestComplexType, TestSize> original;

		for (std::size_t i = 0; i < TestSize; ++i)
		{
			original[i].Value = static_cast<int>(i + 1);
		}

		Baroque::FixedArray<TestComplexType, TestSize> moved(std::move(original));

		for (int i = 0; i < TestSize; ++i)
		{
			EXPECT_EQ(moved[i].Value, i + 1);
		}

		for (std::size_t i = 0; i < TestSize; ++i)
		{
			EXPECT_FALSE(original[i].DoDtor);
		}
	}

	EXPECT_EQ(TestComplexType::CtorCount, TestSize * 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, TestSize);
	EXPECT_EQ(TestComplexType::DtorCount, TestSize);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(FixedArray, CopyAssignmentSimpleTypes)
{
	Baroque::FixedArray<int, TestSize> original;

	for (int i = 0; i < TestSize; ++i)
	{
		original[i] = i + 1;
	}

	Baroque::FixedArray<int, TestSize> copy = original;

	for (int i = 0; i < TestSize; ++i)
	{
		EXPECT_EQ(copy[i], i + 1);
	}
}

TEST(FixedArray, CopyAssignmentComplexTypes)
{
	TestComplexType::Reset();

	{
		Baroque::FixedArray<TestComplexType, TestSize> original;

		for (int i = 0; i < TestSize; ++i)
		{
			original[i].Value = i + 1;
		}

		Baroque::FixedArray<TestComplexType, TestSize> copy = original;

		for (int i = 0; i < TestSize; ++i)
		{
			EXPECT_EQ(copy[i].Value, i + 1);
		}
	}

	EXPECT_EQ(TestComplexType::CtorCount, TestSize * 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, TestSize);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, TestSize * 2);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(FixedArray, MoveAssignmentSimpleTypes)
{
	Baroque::FixedArray<int, TestSize> original;

	for (int i = 0; i < TestSize; ++i)
	{
		original[i] = i + 1;
	}

	Baroque::FixedArray<int, TestSize> moved = std::move(original);

	for (int i = 0; i < TestSize; ++i)
	{
		EXPECT_EQ(moved[i], i + 1);
	}
}

TEST(FixedArray, MoveAssignmentComplexTypes)
{
	TestComplexType::Reset();

	{
		Baroque::FixedArray<TestComplexType, TestSize> original;

		for (int i = 0; i < TestSize; ++i)
		{
			original[i].Value = i + 1;
		}

		Baroque::FixedArray<TestComplexType, TestSize> moved = std::move(original);

		for (int i = 0; i < TestSize; ++i)
		{
			EXPECT_EQ(moved[i].Value, i + 1);
		}

		for (std::size_t i = 0; i < TestSize; ++i)
		{
			EXPECT_FALSE(original[i].DoDtor);
		}
	}

	EXPECT_EQ(TestComplexType::CtorCount, TestSize * 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, TestSize);
	EXPECT_EQ(TestComplexType::DtorCount, TestSize);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(FixedArray, DeleteAllForPointerType)
{
	TestComplexType::Reset();

	Baroque::FixedArray<TestComplexType*, TestSize> pointerArray;

	for (std::size_t i = 0; i < TestSize; ++i)
	{
		pointerArray[i] = new TestComplexType(static_cast<int>(i + 1));
	}

	pointerArray.DeleteAll();

	EXPECT_EQ(TestComplexType::CtorCount, TestSize);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, TestSize);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(FixedArray, ForRange)
{
	static constexpr int expected[TestSize] = {
		0 * 2,
		1 * 2,
		2 * 2,
		3 * 2,
		4 * 2,
		5 * 2,
		6 * 2,
		7 * 2,
	};

	Baroque::FixedArray<int, TestSize> testArray;
	for (std::size_t i = 0; i < TestSize; ++i)
	{
		testArray[i] = expected[i];
	}

	std::size_t index = 0;
	for (auto entry : testArray)
	{
		EXPECT_EQ(entry, expected[index]);
		++index;
	}
}

TEST(FixedArray, Size)
{
	Baroque::FixedArray<int, TestSize> test;
	EXPECT_EQ(test.Size(), TestSize);
}

TEST(FixedArray, Contains)
{
	Baroque::FixedArray<int, TestSize> test;

	EXPECT_FALSE(test.Contains(42));

	test[2] = 42;

	EXPECT_TRUE(test.Contains(42));
}

TEST(FixedArray, ContainsByPredicate)
{
	Baroque::FixedArray<TestComplexType, TestSize> test;

	auto predicate = [](const auto& item) {
		return item.Value == 2;
	};

	EXPECT_FALSE(test.ContainsByPredicate(predicate));

	test[0].Value = 2;

	EXPECT_TRUE(test.ContainsByPredicate(predicate));
}

TEST(FixedArray, IndexOf)
{
	Baroque::FixedArray<int, TestSize> test;

	EXPECT_EQ(test.IndexOf(42), TestSize);

	test[2] = 42;

	EXPECT_EQ(test.IndexOf(42), 2);
}

TEST(FixedArray, IndexOfByPredicate)
{
	Baroque::FixedArray<TestComplexType, TestSize> test;

	auto predicate = [](const auto& item) {
		return item.Value == 69;
	};

	EXPECT_EQ(test.IndexOfByPredicate(predicate), TestSize);

	test[2].Value = 69;

	EXPECT_EQ(test.IndexOfByPredicate(predicate), 2);
}

TEST(FixedArray, Find)
{
	Baroque::FixedArray<TestComplexType, TestSize> test;

	EXPECT_EQ(test.Find(1), nullptr);

	test[2].Value = 200;

	auto foundIt = test.Find(200);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt->Value, 200);
}

TEST(FixedArray, FindByPredicate)
{
	Baroque::FixedArray<TestComplexType, TestSize> test;

	auto predicate = [](const auto& item) {
		return item.Value == 200;
	};

	EXPECT_EQ(test.FindByPredicate(predicate), nullptr);

	test[2].Value = 200;

	auto foundIt = test.FindByPredicate(predicate);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt->Value, 200);
}

TEST(FixedArray, Front)
{
	Baroque::FixedArray<TestComplexType, TestSize> test;

	test[0].Value = 200;

	EXPECT_EQ(test.Front().Value, 200);
}

TEST(FixedArray, Last)
{
	Baroque::FixedArray<TestComplexType, TestSize> test;

	test[TestSize - 1] = 200;

	EXPECT_EQ(test.Last(), 200);
}

TEST(FixedArray, ConvertToArrayView)
{
	Baroque::FixedArray<int, TestSize> test;

	auto arrayView = test.ToArrayView();

	EXPECT_EQ(test.Data(), arrayView.Data());
	EXPECT_EQ(test.Size(), arrayView.Size());
}

TEST(FixedArray, ConvertToArraySpan)
{
	Baroque::FixedArray<int, TestSize> test;

	auto arraySpan = test.ToArraySpan();

	EXPECT_EQ(test.Data(), arraySpan.Data());
	EXPECT_EQ(test.Size(), arraySpan.Size());
}

TEST(FixedArray, Slice)
{
	Baroque::FixedArray<int, TestSize> full;

	for (std::size_t i = 0; i < TestSize; ++i)
	{
		full[i] = static_cast<int>(i + 1);
	}

	auto slice = full.Slice(1, 3);

	EXPECT_EQ(slice.Data(), full.Data() + 1);
	EXPECT_EQ(slice.Size(), 2);

	for (std::size_t i = 0; i < slice.Size(); ++i)
	{
		EXPECT_EQ(slice[i], i + 2);
	}
}

TEST(FixedArray, Subset)
{
	Baroque::FixedArray<int, TestSize> full;

	for (std::size_t i = 0; i < TestSize; ++i)
	{
		full[i] = static_cast<int>(i + 1);
	}

	auto subset = full.Subset(3);

	EXPECT_EQ(subset.Data(), full.Data() + 3);
	EXPECT_EQ(subset.Size(), TestSize - 3);

	for (std::size_t i = 0; i < subset.Size(); ++i)
	{
		EXPECT_EQ(subset[i], i + 4);
	}

	auto subsetWithCount = full.Subset(1, 2);

	EXPECT_EQ(subsetWithCount.Data(), full.Data() + 1);
	EXPECT_EQ(subsetWithCount.Size(), 2);

	for (std::size_t i = 0; i < subset.Size(); ++i)
	{
		EXPECT_EQ(subsetWithCount[i], i + 2);
	}
}

TEST(FixedArray, OperatorEquals)
{
	Baroque::FixedArray<int, TestSize> left;
	Baroque::FixedArray<int, TestSize> right;

	EXPECT_EQ(left, right);

	for (std::size_t i = 0; i < TestSize; ++i)
	{
		left[i] = static_cast<int>(i + 1);
		right[i] = static_cast<int>(i + 1);
	}

	EXPECT_EQ(left, right);
}

TEST(FixedArray, OperatorNotEquals)
{
	Baroque::FixedArray<int, TestSize> left;
	Baroque::FixedArray<int, TestSize> right;

	for (std::size_t i = 0; i < TestSize; ++i)
	{
		left[i] = static_cast<int>(i + 1);
	}

	EXPECT_NE(left, right);

	for (std::size_t i = 0; i < TestSize; ++i)
	{
		right[i] = static_cast<int>(i * 2);
	}

	EXPECT_NE(left, right);
}

TEST(FixedArray, InitilializerListCtorWithSimpleType)
{
	Baroque::FixedArray<int, TestSize> testInit{ 1, 2, 3, 4, 5, 6, 7, 8 };

	for (std::size_t i = 0; i < TestSize; ++i)
	{
		EXPECT_EQ(testInit[i], i + 1);
	}

	Baroque::FixedArray<int, TestSize> notFullyInit{ 1,2,3 };

	for (std::size_t i = 0; i < 3; ++i)
	{
		EXPECT_EQ(notFullyInit[i], i + 1);
	}

	for (std::size_t i = 3; i < TestSize; ++i)
	{
		EXPECT_EQ(notFullyInit[i], 0);
	}
}

TEST(FixedArray, InitilializerListAssignWithSimpleType)
{
	Baroque::FixedArray<int, TestSize> testInit;
	
	testInit = { 1, 2, 3, 4, 5, 6, 7, 8 };

	for (std::size_t i = 0; i < TestSize; ++i)
	{
		EXPECT_EQ(testInit[i], i + 1);
	}

	Baroque::FixedArray<int, TestSize> notFullyInit;
	
	notFullyInit = { 1,2,3 };

	for (std::size_t i = 0; i < 3; ++i)
	{
		EXPECT_EQ(notFullyInit[i], i + 1);
	}

	for (std::size_t i = 3; i < TestSize; ++i)
	{
		EXPECT_EQ(notFullyInit[i], 0);
	}
}

TEST(FixedArray, MakeFixedArray)
{
	auto result = Baroque::MakeFixedArray(1, 2, 3, 4, 5);

	bool isSame = std::is_same_v<decltype(result), Baroque::FixedArray<int, 5>>;
	EXPECT_TRUE(isSame);

	EXPECT_EQ(result.Size(), 5);

	for (std::size_t i = 0; i < result.Size(); ++i)
	{
		EXPECT_EQ(result[i], i + 1);
	}
}