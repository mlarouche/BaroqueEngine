#include <gtest/gtest.h>

#include "Core/Containers/ArraySpan.h"

namespace
{
	int TestArray[5] = {
		100,
		200,
		300,
		400,
		500
	};

	int SimilarArray[5] = {
		100,
		200,
		300,
		400,
		500
	};

	int NotSameSize[3] = {
		100,
		200,
		300
	};

	int DifferentContent[5] = {
		1,
		2,
		3,
		4,
		5
	};
}

TEST(ArraySpan, DefaultArrayViewShouldHaveNullBeginAndEnd)
{
	Baroque::ArraySpan<int> null;

	EXPECT_TRUE(null.IsEmpty());
	EXPECT_TRUE(null.IsNull());

	EXPECT_EQ(null.begin(), nullptr);
	EXPECT_EQ(null.end(), nullptr);
}

TEST(ArraySpan, IsEmptyShouldWorkWithArrayOfZeroSize)
{
	int ZeroInt = 0;

	Baroque::ArraySpan<int> span(&ZeroInt, &ZeroInt);

	EXPECT_TRUE(span.IsEmpty());
	EXPECT_FALSE(span.IsNull());
}
TEST(ArraySpan, ShouldConstructFromANativeArray)
{
	Baroque::ArraySpan<int> native(TestArray);

	EXPECT_EQ(native.Size(), 5);
	EXPECT_EQ(native.Data(), TestArray);

	for (std::size_t i = 0; i < native.Size(); ++i)
	{
		EXPECT_EQ(native[i], (i + 1) * 100);
	}
}

TEST(ArraySpan, ShouldConstructFromTwoPointer)
{
	Baroque::ArraySpan<int> slice(TestArray + 1, TestArray + 3);

	EXPECT_EQ(slice.Size(), 2);

	EXPECT_EQ(slice[0], 200);
	EXPECT_EQ(slice[1], 300);
}

TEST(ArraySpan, ShouldConstructFromPointerAndSize)
{
	Baroque::ArraySpan<int> slice(TestArray + 1, 3);

	EXPECT_EQ(slice.Size(), 3);

	EXPECT_EQ(slice[0], 200);
	EXPECT_EQ(slice[1], 300);
	EXPECT_EQ(slice[2], 400);
}

TEST(ArraySpan, CopyCtor)
{
	Baroque::ArraySpan<int> original(TestArray);
	Baroque::ArraySpan<int> copy(original);

	EXPECT_EQ(copy.begin(), original.begin());
	EXPECT_EQ(copy.end(), original.end());
}

TEST(ArraySpan, CopyAssignment)
{
	Baroque::ArraySpan<int> original(TestArray);
	Baroque::ArraySpan<int> copy;

	copy = original;

	EXPECT_EQ(copy.begin(), original.begin());
	EXPECT_EQ(copy.end(), original.end());
}

TEST(ArraySpan, ForRangeLoop)
{
	Baroque::ArraySpan<int> test(TestArray);

	std::size_t index = 0;
	for (auto entry : test)
	{
		EXPECT_EQ(entry, TestArray[index]);
		++index;
	}
}

TEST(ArraySpan, Contains)
{
	Baroque::ArraySpan<int> test(TestArray);

	EXPECT_FALSE(test.Contains(1));
	EXPECT_TRUE(test.Contains(200));
}

TEST(ArraySpan, ContainsByPredicate)
{
	Baroque::ArraySpan<int> test(TestArray);

	EXPECT_FALSE(test.ContainsByPredicate([](const auto& item) { return item == 2; }));
	EXPECT_TRUE(test.ContainsByPredicate([](const auto& item) { return item == 200; }));
}

TEST(ArraySpan, IndexOf)
{
	Baroque::ArraySpan<int> test(TestArray);

	EXPECT_EQ(test.IndexOf(1), test.Size());
	EXPECT_EQ(test.IndexOf(300), 2);
}

TEST(ArraySpan, IndexOfByPredicate)
{
	Baroque::ArraySpan<int> test(TestArray);

	auto firstIndexOf = test.IndexOfByPredicate([](const auto& item) { return item == 2; });
	auto secondIndexOf = test.IndexOfByPredicate([](const auto& item) { return item == 300; });
	EXPECT_EQ(firstIndexOf, test.Size());
	EXPECT_EQ(secondIndexOf, 2);
}

TEST(ArraySpan, Find)
{
	Baroque::ArraySpan<int> test(TestArray);

	EXPECT_EQ(test.Find(1), nullptr);

	auto foundIt = test.Find(200);
	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*foundIt, 200);
}

TEST(ArraySpan, FindByPredicate)
{
	Baroque::ArraySpan<int> test(TestArray);

	auto notFoundPredicate = [](const auto& item) {
		return item == 1;
	};

	auto foundPredicate = [](const auto& item) {
		return item == 200;
	};

	EXPECT_EQ(test.FindByPredicate(notFoundPredicate), nullptr);

	auto foundIt = test.FindByPredicate(foundPredicate);
	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*foundIt, 200);
}

TEST(ArraySpan, Front)
{
	Baroque::ArraySpan<int> test(TestArray);

	EXPECT_EQ(test.Front(), 100);
}

TEST(ArraySpan, Last)
{
	Baroque::ArraySpan<int> test(TestArray);

	EXPECT_EQ(test.Last(), 500);
}

TEST(ArraySpan, Slice)
{
	Baroque::ArraySpan<int> full(TestArray);

	auto slice = full.Slice(1, 3);

	EXPECT_EQ(slice.Size(), 2);
	EXPECT_EQ(slice.begin(), TestArray + 1);

	EXPECT_EQ(slice[0], 200);
	EXPECT_EQ(slice[1], 300);
}

TEST(ArraySpan, Subset)
{
	Baroque::ArraySpan<int> full(TestArray);

	auto subset = full.Subset(3);

	EXPECT_EQ(subset.Size(), 2);
	EXPECT_EQ(subset[0], 400);
	EXPECT_EQ(subset[1], 500);

	auto subsetWithCount = full.Subset(1, 3);

	EXPECT_EQ(subsetWithCount.Size(), 3);
	EXPECT_EQ(subsetWithCount[0], 200);
	EXPECT_EQ(subsetWithCount[1], 300);
	EXPECT_EQ(subsetWithCount[2], 400);
}

TEST(ArraySpan, OperatorEquals)
{
	Baroque::ArraySpan<int> left(TestArray);
	Baroque::ArraySpan<int> right(SimilarArray);

	EXPECT_EQ(left, right);
}

TEST(ArraySpan, OperatorNotEquals)
{
	Baroque::ArraySpan<int> test(TestArray);
	Baroque::ArraySpan<int> notSameSize(NotSameSize);
	Baroque::ArraySpan<int> different(DifferentContent);

	EXPECT_NE(test, notSameSize);
	EXPECT_NE(test, different);
}
