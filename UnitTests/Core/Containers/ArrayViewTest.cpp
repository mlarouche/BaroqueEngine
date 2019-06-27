#include <gtest/gtest.h>

#include "Core/Containers/ArrayView.h"

namespace
{
	const int TestArray[5] = {
		100,
		200,
		300,
		400,
		500
	};

	const int SimilarArray[5] = {
		100,
		200,
		300,
		400,
		500
	};

	const int NotSameSize[3] = {
		100,
		200,
		300
	};

	const int DifferentContent[5] = {
		1,
		2,
		3,
		4,
		5
	};
}

TEST(ArrayView, DefaultArrayViewShouldHaveNullBeginAndEnd)
{
	Baroque::ArrayView<int> null;

	std::string_view test;

	EXPECT_EQ(null.begin(), nullptr);
	EXPECT_EQ(null.end(), nullptr);
}

TEST(ArrayView, ShouldConstructFromANativeArray)
{
	Baroque::ArrayView<int> native(TestArray);

	EXPECT_EQ(native.Size(), 5);
	EXPECT_EQ(native.Data(), TestArray);

	for (std::size_t i = 0; i < native.Size(); ++i)
	{
		EXPECT_EQ(native[i], (i + 1) * 100);
	}
}

TEST(ArrayView, ShouldConstructFromTwoPointer)
{
	Baroque::ArrayView<int> slice(TestArray + 1, TestArray + 3);

	EXPECT_EQ(slice.Size(), 2);

	EXPECT_EQ(slice[0], 200);
	EXPECT_EQ(slice[1], 300);
}

TEST(ArrayView, ShouldConstructFromPointerAndSize)
{
	Baroque::ArrayView<int> slice(TestArray + 1, 3);

	EXPECT_EQ(slice.Size(), 3);

	EXPECT_EQ(slice[0], 200);
	EXPECT_EQ(slice[1], 300);
	EXPECT_EQ(slice[2], 400);
}

TEST(ArrayView, CopyCtor)
{
	Baroque::ArrayView<int> original(TestArray);
	Baroque::ArrayView<int> copy(original);

	EXPECT_EQ(copy.begin(), original.begin());
	EXPECT_EQ(copy.end(), original.end());
}

TEST(ArrayView, CopyAssignment)
{
	Baroque::ArrayView<int> original(TestArray);
	Baroque::ArrayView<int> copy;

	copy = original;

	EXPECT_EQ(copy.begin(), original.begin());
	EXPECT_EQ(copy.end(), original.end());
}

TEST(ArrayView, ForRangeLoop)
{
	Baroque::ArrayView<int> test(TestArray);

	std::size_t index = 0;
	for (auto entry : test)
	{
		EXPECT_EQ(entry, TestArray[index]);
		++index;
	}
}

TEST(ArrayView, Contains)
{
	Baroque::ArrayView<int> test(TestArray);

	EXPECT_FALSE(test.Contains(1));
	EXPECT_TRUE(test.Contains(200));
}

TEST(ArrayView, IndexOf)
{
	Baroque::ArrayView<int> test(TestArray);

	EXPECT_EQ(test.IndexOf(1), test.Size());
	EXPECT_EQ(test.IndexOf(300), 2);
}

TEST(ArrayView, Slice)
{
	Baroque::ArrayView<int> full(TestArray);

	auto slice = full.Slice(1, 3);

	EXPECT_EQ(slice.Size(), 2);
	EXPECT_EQ(slice.begin(), TestArray + 1);

	EXPECT_EQ(slice[0], 200);
	EXPECT_EQ(slice[1], 300);
}

TEST(ArrayView, Subset)
{
	Baroque::ArrayView<int> full(TestArray);

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

TEST(ArrayView, OperatorEquals)
{
	Baroque::ArrayView<int> left(TestArray);
	Baroque::ArrayView<int> right(SimilarArray);

	EXPECT_EQ(left, right);
}

TEST(ArrayView, OperatorNotEquals)
{
	Baroque::ArrayView<int> test(TestArray);
	Baroque::ArrayView<int> notSameSize(NotSameSize);
	Baroque::ArrayView<int> different(DifferentContent);

	EXPECT_NE(test, notSameSize);
	EXPECT_NE(test, different);
}