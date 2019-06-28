#include <gtest/gtest.h>

#include "Core/Containers/Array.h"
#include "UnitTests/Core/TestComplexType.h"

namespace
{
	class VerifyDeallocImpl : public Baroque::Memory::MallocAllocator
	{
	public:
		void* Allocate(const std::size_t size)
		{
			return MallocAllocator::Allocate(size);
		}

		void Deallocate(void* ptr)
		{
			LastDeallocation = ptr;
			MallocAllocator::Deallocate(ptr);
		}

		static void Reset()
		{
			LastDeallocation = nullptr;
		}

		static void* LastDeallocation;
	};

	struct Test
	{
		int IntValue = 42;
		float FloatValue = 3.1415f;
	};

	const int TestConstArray[] = {
		100,
		200,
		300,
		400,
		500
	};

	int TestArray[] = {
		10,
		20,
		30,
		40,
		50
	};

	BAROQUE_DEFINE_ALLOCATOR(VerifyDealloc, VerifyDeallocImpl);

	void* VerifyDeallocImpl::LastDeallocation = nullptr;
}

TEST(Array, ShouldProperlyInitANullArray)
{
	Baroque::Array<int> null;

	EXPECT_EQ(null.Data(), nullptr);
	EXPECT_EQ(null.Size(), 0);
	EXPECT_EQ(null.Capacity(), 0);
}

TEST(Array, ShouldProperlyInitWithInitialSize)
{
	Baroque::Array<int> valid(5);

	EXPECT_NE(valid.Data(), nullptr);
	EXPECT_EQ(valid.Size(), 5);
	EXPECT_EQ(valid.Capacity(), 5);
}

TEST(Array, ShouldZeroSimpleArrayType)
{
	Baroque::Array<int> valid(5);

	for (std::size_t i = 0; i < valid.Size(); ++i)
	{
		EXPECT_EQ(valid[i], 0);
	}
}

TEST(Array, ShouldInitWithInitialValueAndSize)
{
	Baroque::Array<int> valid(5, 42);

	for (std::size_t i = 0; i < valid.Size(); ++i)
	{
		EXPECT_EQ(valid[i], 42);
	}
}

TEST(Array, ShouldInitWithInitializerList)
{
	Baroque::Array<int> valid{ 100,200,300,400,500 };

	EXPECT_EQ(valid.Size(), 5);
	EXPECT_EQ(valid.Capacity(), 5);

	for (std::size_t i = 0; i < valid.Size(); ++i)
	{
		EXPECT_EQ(valid[i], (i + 1) * 100);
	}
}

TEST(Array, ShouldCallComplexTypeCtorWithInitialSize)
{
	TestComplexType::Reset();

	Baroque::Array<TestComplexType> array(5);

	EXPECT_EQ(TestComplexType::CtorCount, 5);
	EXPECT_EQ(TestComplexType::DtorCount, 0);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(Array, ShouldDeallocateDataProperly)
{
	VerifyDeallocImpl::Reset();

	void* data = nullptr;

	{
		Baroque::Array<int, VerifyDealloc> array(5);
		data = array.Data();
	}

	EXPECT_NE(data, nullptr);
	EXPECT_NE(VerifyDeallocImpl::LastDeallocation, nullptr);

	EXPECT_EQ(data, VerifyDeallocImpl::LastDeallocation);
}

TEST(Array, ShouldCallDestructorWithComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::Array<TestComplexType> array(5);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 5);
	EXPECT_EQ(TestComplexType::DtorCount, 5);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(Array, CopyConstructorWithSimpleType)
{
	Baroque::Array<int> original(5);

	for (std::size_t i = 0; i < original.Size(); ++i)
	{
		original[i] = static_cast<int>((i + 1) * 200);
	}

	Baroque::Array<int> copy(original);

	EXPECT_NE(copy.Data(), original.Data());
	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());

	for (std::size_t i = 0; i < original.Size(); ++i)
	{
		EXPECT_EQ(copy[i], (i + 1) * 200);
	}
}

TEST(Array, CopyConstructorWithComplexType)
{
	TestComplexType::Reset();

	{
		Baroque::Array<TestComplexType> original(5);
		for (std::size_t i = 0; i < original.Size(); ++i)
		{
			original[i].DummyValue = static_cast<int>((i + 1) * 100);
		}

		Baroque::Array<TestComplexType> copy(original);

		EXPECT_NE(copy.Data(), original.Data());
		EXPECT_EQ(copy.Size(), original.Size());
		EXPECT_EQ(copy.Capacity(), original.Capacity());

		for (std::size_t i = 0; i < original.Size(); ++i)
		{
			EXPECT_EQ(copy[i].DummyValue, (i + 1) * 100);
		}
	}

	EXPECT_EQ(TestComplexType::CtorCount, 5);
	EXPECT_EQ(TestComplexType::DtorCount, 10);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 5);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(Array, CopyConstructorWithDifferentAllocator)
{
	Baroque::Array<int, VerifyDealloc> original(5);

	for (std::size_t i = 0; i < original.Size(); ++i)
	{
		original[i] = static_cast<int>((i + 1) * 200);
	}

	Baroque::Array<int> copy(original);

	EXPECT_NE(copy.Data(), original.Data());
	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());

	for (std::size_t i = 0; i < original.Size(); ++i)
	{
		EXPECT_EQ(copy[i], (i + 1) * 200);
	}
}

TEST(Array, MoveConstructorShouldWork)
{
	Baroque::Array<int> original(5);

	for (std::size_t i = 0; i < original.Size(); ++i)
	{
		original[i] = static_cast<int>((i + 1) * 200);
	}

	auto previousData = original.Data();

	Baroque::Array<int> moved(std::move(original));

	EXPECT_EQ(original.Data(), nullptr);
	EXPECT_EQ(original.Size(), 0);
	EXPECT_EQ(original.Capacity(), 0);

	EXPECT_EQ(moved.Data(), previousData);
	EXPECT_EQ(moved.Size(), 5);
	EXPECT_EQ(moved.Capacity(), 5);

	for (std::size_t i = 0; i < original.Size(); ++i)
	{
		EXPECT_EQ(moved[i], (i + 1) * 200);
	}
}

TEST(Array, CopyAssignmentWithSimpleType)
{
	Baroque::Array<int> original(5);

	for (std::size_t i = 0; i < original.Size(); ++i)
	{
		original[i] = static_cast<int>((i + 1) * 200);
	}

	Baroque::Array<int> copy;

	copy = original;

	EXPECT_NE(copy.Data(), original.Data());
	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());

	for (std::size_t i = 0; i < original.Size(); ++i)
	{
		EXPECT_EQ(copy[i], (i + 1) * 200);
	}
}

TEST(Array, CopyAssignmentShouldDeletePreviousAllocationAndDeleteComplexType)
{
	TestComplexType::Reset();
	VerifyDeallocImpl::Reset();

	{
		Baroque::Array<TestComplexType> original(5);

		for (std::size_t i = 0; i < original.Size(); ++i)
		{
			original[i].DummyValue = static_cast<int>((i + 1) * 200);
		}

		Baroque::Array<TestComplexType, VerifyDealloc> copy(8);

		auto* previousData = copy.Data();

		EXPECT_EQ(copy.Size(), 8);
		EXPECT_EQ(copy.Capacity(), 8);

		copy = original;

		EXPECT_EQ(VerifyDeallocImpl::LastDeallocation, previousData);

		EXPECT_NE(copy.Data(), original.Data());
		EXPECT_EQ(copy.Size(), original.Size());
		EXPECT_EQ(copy.Capacity(), original.Capacity());

		for (std::size_t i = 0; i < original.Size(); ++i)
		{
			EXPECT_EQ(copy[i].DummyValue, (i + 1) * 200);
		}
	}

	EXPECT_EQ(TestComplexType::CtorCount, 5 + 8);
	EXPECT_EQ(TestComplexType::DtorCount, 5 * 2 + 8);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 5);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(Array, MoveAssignmentShouldWork)
{
	Baroque::Array<int> original(5);

	for (std::size_t i = 0; i < original.Size(); ++i)
	{
		original[i] = static_cast<int>((i + 1) * 200);
	}

	auto previousData = original.Data();

	Baroque::Array<int> moved;
	
	moved = std::move(original);

	EXPECT_EQ(original.Data(), nullptr);
	EXPECT_EQ(original.Size(), 0);
	EXPECT_EQ(original.Capacity(), 0);

	EXPECT_EQ(moved.Data(), previousData);
	EXPECT_EQ(moved.Size(), 5);
	EXPECT_EQ(moved.Capacity(), 5);

	for (std::size_t i = 0; i < original.Size(); ++i)
	{
		EXPECT_EQ(moved[i], (i + 1) * 200);
	}
}

TEST(Array, MoveAssignmentShouldDeletePreviousContent)
{
	TestComplexType::Reset();
	VerifyDeallocImpl::Reset();

	{
		Baroque::Array<TestComplexType, VerifyDealloc> original(5);

		for (std::size_t i = 0; i < original.Size(); ++i)
		{
			original[i].DummyValue = static_cast<int>((i + 1) * 200);
		}

		auto* originalData = original.Data();

		Baroque::Array<TestComplexType, VerifyDealloc> moved(8);

		auto* previousData = moved.Data();

		EXPECT_EQ(moved.Size(), 8);
		EXPECT_EQ(moved.Capacity(), 8);

		moved = std::move(original);

		EXPECT_EQ(VerifyDeallocImpl::LastDeallocation, previousData);

		EXPECT_EQ(original.Data(), nullptr);
		EXPECT_EQ(original.Size(), 0);
		EXPECT_EQ(original.Capacity(), 0);

		EXPECT_EQ(moved.Data(), originalData);
		EXPECT_EQ(moved.Size(), 5);
		EXPECT_EQ(moved.Capacity(), 5);

		for (std::size_t i = 0; i < original.Size(); ++i)
		{
			EXPECT_EQ(moved[i].DummyValue, (i + 1) * 200);
		}
	}

	EXPECT_EQ(TestComplexType::CtorCount, 5 + 8);
	EXPECT_EQ(TestComplexType::DtorCount, 5 + 8);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(Array, ShouldAddByCopy)
{
	TestComplexType::Reset();

	{
		Baroque::Array<TestComplexType> array;

		EXPECT_EQ(array.Data(), nullptr);
		EXPECT_EQ(array.Size(), 0);
		EXPECT_EQ(array.Capacity(), 0);

		TestComplexType item{ 123 };
		array.Add(item);

		EXPECT_NE(array.Data(), nullptr);
		EXPECT_EQ(array[0].DummyValue, 123);
		EXPECT_EQ(array.Size(), 1);
		EXPECT_GT(array.Capacity(), 1);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::DtorCount, 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 1);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(Array, ShouldAddByMove)
{
	TestComplexType::Reset();

	{
		Baroque::Array<TestComplexType> array;

		EXPECT_EQ(array.Data(), nullptr);
		EXPECT_EQ(array.Size(), 0);
		EXPECT_EQ(array.Capacity(), 0);

		array.Add(TestComplexType{ 345 });

		EXPECT_NE(array.Data(), nullptr);
		EXPECT_EQ(array[0].DummyValue, 345);
		EXPECT_EQ(array.Size(), 1);
		EXPECT_GT(array.Capacity(), 1);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::DtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(Array, AddByInitializerList)
{
	Baroque::Array<int> array;

	array.Add(1);
	array.Add({ 2,3,4,5,6,7,8 });

	EXPECT_EQ(array.Size(), 8);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i], i + 1);
	}
}

TEST(Array, AddByArrayView)
{
	Baroque::Array<int> array;

	array.Add(TestConstArray);

	EXPECT_EQ(array.Size(), sizeof(TestConstArray) / sizeof(TestConstArray[0]));

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i], TestConstArray[i]);
	}
}

TEST(Array, AddByArraySpan)
{
	Baroque::Array<int> array;

	array.Add(Baroque::ArraySpan{ TestArray });

	EXPECT_EQ(array.Size(), sizeof(TestArray) / sizeof(TestArray[0]));

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i], TestArray[i]);
	}
}

TEST(Array, ForRangeLoop)
{
	Baroque::Array<int> array;

	array.Add(TestConstArray);

	std::size_t index = 0;
	for (auto entry : array)
	{
		EXPECT_EQ(entry, TestConstArray[index]);
		++index;
	}
}

TEST(Array, IsEmpty)
{
	Baroque::Array<int> array;

	EXPECT_TRUE(array.IsEmpty());

	array.Add(1);

	EXPECT_FALSE(array.IsEmpty());
}

TEST(Array, Contains)
{
	Baroque::Array<int> array;

	EXPECT_FALSE(array.Contains(3));

	array.Add(3);

	EXPECT_TRUE(array.Contains(3));
}

TEST(Array, ContainsByPredicate)
{
	Baroque::Array<TestComplexType> array;

	auto predicate = [](const TestComplexType& item) {
		return item.DummyValue == 69;
	};

	EXPECT_FALSE(array.ContainsByPredicate(predicate));

	array.Add(TestComplexType{ 69 });

	EXPECT_TRUE(array.ContainsByPredicate(predicate));
}

TEST(Array, Find)
{
	Baroque::Array<int> array;

	EXPECT_EQ(array.Find(3), nullptr);

	array.Add(1);
	array.Add(3);

	auto foundIt = array.Find(3);
	EXPECT_EQ(foundIt, array.Data() + 1);
	EXPECT_EQ(*foundIt, 3);
}

TEST(Array, FindByPredicate)
{
	Baroque::Array<TestComplexType> array;

	auto predicate = [](const TestComplexType& item) {
		return item.DummyValue == 69;
	};

	EXPECT_EQ(array.FindByPredicate(predicate), nullptr);

	array.Add(TestComplexType{ 12 });
	array.Add(TestComplexType{ 69 });

	auto foundIt = array.FindByPredicate(predicate);
	EXPECT_EQ(foundIt, array.Data() + 1);
	EXPECT_EQ(foundIt->DummyValue, 69);
}

TEST(Array, IndexOf)
{
	Baroque::Array<int> array;

	EXPECT_EQ(array.IndexOf(4), array.Size());

	array.Add({ 5,4,3,2,1 });

	EXPECT_EQ(array.IndexOf(4), 1);
	EXPECT_EQ(array.IndexOf(1), 4);
}

TEST(Array, IndexOfByPredicate)
{
	Baroque::Array<TestComplexType> array;

	auto predicate = [](const TestComplexType& item) {
		return item.DummyValue == 69;
	};

	EXPECT_EQ(array.IndexOfByPredicate(predicate), array.Size());

	array.Add(TestComplexType{ 12 });
	array.Add(TestComplexType{ 23 });
	array.Add(TestComplexType{ 69 });
	array.Add(TestComplexType{ 45 });

	EXPECT_EQ(array.IndexOfByPredicate(predicate), 2);
}

TEST(Array, AddUniqueByCopy)
{
	TestComplexType::Reset();

	{
		Baroque::Array<TestComplexType> array;

		TestComplexType item{ 56 };
		EXPECT_TRUE(array.AddUnique(item));
		EXPECT_FALSE(array.AddUnique(item));
	}

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::DtorCount, 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 1);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(Array, AddUniqueByMove)
{
	TestComplexType::Reset();

	{
		Baroque::Array<TestComplexType> array;

		EXPECT_TRUE(array.AddUnique(TestComplexType{ 56 }));
		EXPECT_FALSE(array.AddUnique(TestComplexType{ 56 }));
	}

	EXPECT_EQ(TestComplexType::CtorCount, 2);
	EXPECT_EQ(TestComplexType::DtorCount, 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(Array, Emplace)
{
	Baroque::Array<TestComplexType> array;

	array.Emplace();

	EXPECT_EQ(array.Size(), 1);
	EXPECT_EQ(array[0].DummyValue, 42);

	array.Emplace(69);

	EXPECT_EQ(array.Size(), 2);
	EXPECT_EQ(array[1].DummyValue, 69);
}

// Insert one value
TEST(Array, InsertByCopy)
{
	TestComplexType::Reset();

	const TestComplexType ExpectedResults[] {
		TestComplexType{1},
		TestComplexType{4},
		TestComplexType{2},
		TestComplexType{3}
	};
	
	{
		Baroque::Array<TestComplexType> array;

		array.Add(ExpectedResults[0]);
		array.Add(ExpectedResults[2]);
		array.Add(ExpectedResults[3]);

		array.Insert(1, ExpectedResults[1]);

		EXPECT_EQ(array[1].DummyValue, 4);
		EXPECT_EQ(array.Size(), 4);

		for (std::size_t i = 0; i < array.Size(); ++i)
		{
			EXPECT_EQ(array[i].DummyValue, ExpectedResults[i].DummyValue);
		}
	}

	EXPECT_EQ(TestComplexType::CtorCount, 4);
	EXPECT_EQ(TestComplexType::DtorCount, 4);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 4);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 2);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(Array, InsertByMove)
{
	TestComplexType::Reset();

	const TestComplexType ExpectedResults[]{
		TestComplexType{1},
		TestComplexType{4},
		TestComplexType{2},
		TestComplexType{3}
	};

	{
		Baroque::Array<TestComplexType> array;

		array.Add(TestComplexType{1});
		array.Add(TestComplexType{2});
		array.Add(TestComplexType{3});

		array.Insert(1, TestComplexType{4});

		EXPECT_EQ(array[1].DummyValue, 4);
		EXPECT_EQ(array.Size(), 4);

		for (std::size_t i = 0; i < array.Size(); ++i)
		{
			EXPECT_EQ(array[i].DummyValue, ExpectedResults[i].DummyValue);
		}
	}

	EXPECT_EQ(TestComplexType::CtorCount, 8);
	EXPECT_EQ(TestComplexType::DtorCount, 4);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 6);
	EXPECT_EQ(TestComplexType::CopyAssignmentCount, 0);
}

TEST(Array, InsertWithEmptyArrayShouldWork)
{
	Baroque::Array<int> array;

	array.Insert(0, 42);

	EXPECT_EQ(array.Size(), 1);
	EXPECT_EQ(array[0], 42);
}

TEST(Array, InsertAtEndShouldWork)
{
	Baroque::Array<int> array;

	array.Add(1);
	array.Add(2);
	array.Add(3);
	array.Add(4);

	array.Insert(array.Size(), 42);

	EXPECT_EQ(array.Size(), 5);
	EXPECT_EQ(array[4], 42);
}

TEST(Array, InsertWithIntializerList)
{
	static const int ExpectedResults[] = {
		1,
		10,
		20,
		30,
		40,
		2,
		3,
		4,
	};

	Baroque::Array<int> array;

	array.Add(1);
	array.Add(2);
	array.Add(3);
	array.Add(4);

	array.Insert(1, { 10, 20, 30, 40 });

	EXPECT_EQ(array.Size(), 8);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i], ExpectedResults[i]);
	}
}

TEST(Array, InsertWithArrayView)
{
	static const int ExpectedResults[] = {
		1,
		10,
		20,
		30,
		40,
		2,
		3,
		4,
	};

	static const int NewValues[] = {
		10, 20, 30, 40
	};

	Baroque::Array<int> array;

	array.Add(1);
	array.Add(2);
	array.Add(3);
	array.Add(4);

	array.Insert(1, Baroque::ArrayView{ NewValues });

	EXPECT_EQ(array.Size(), 8);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i], ExpectedResults[i]);
	}
}

TEST(Array, InsertWithArraySpan)
{
	static const int ExpectedResults[] = {
		1,
		10,
		20,
		30,
		40,
		2,
		3,
		4,
	};

	static int NewValues[] = {
		10, 20, 30, 40
	};

	Baroque::Array<int> array;

	array.Add(1);
	array.Add(2);
	array.Add(3);
	array.Add(4);

	array.Insert(1, Baroque::ArraySpan{ NewValues });

	EXPECT_EQ(array.Size(), 8);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i], ExpectedResults[i]);
	}
}

TEST(Array, EmplaceAt)
{
	const TestComplexType ExpectedResults[]{
		1,
		69,
		42,
		2,
		3,
		4
	};

	Baroque::Array<TestComplexType> array;

	array.Add(1);
	array.Add(2);
	array.Add(3);
	array.Add(4);

	array.EmplaceAt(1);
	array.EmplaceAt(1, 69);

	EXPECT_EQ(array.Size(), 6);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i].DummyValue, ExpectedResults[i].DummyValue);
	}
}

TEST(Array, Clear)
{
	TestComplexType::Reset();

	Baroque::Array<TestComplexType> array;

	array.Add(TestComplexType{ 1 });
	array.Add(TestComplexType{ 2 });
	array.Add(TestComplexType{ 3 });
	array.Add(TestComplexType{ 4 });

	auto previousCapacity = array.Capacity();

	array.Clear();

	EXPECT_EQ(array.Size(), 0);
	EXPECT_EQ(array.Capacity(), previousCapacity);
	EXPECT_EQ(TestComplexType::DtorCount, 4);
}

TEST(Array, RemoveByValue)
{
	static const int ExpectedResults[]{
		1,3,4
	};

	Baroque::Array<TestComplexType> array;

	array.Add(1);
	array.Add(2);
	array.Add(3);
	array.Add(2);
	array.Add(4);

	TestComplexType::Reset();

	auto removed = array.Remove(TestComplexType{ 2 });

	EXPECT_EQ(TestComplexType::DtorCount, 3);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 3);

	EXPECT_EQ(removed, 2);
	EXPECT_EQ(array.Size(), 3);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i].DummyValue, ExpectedResults[i]);
	}
}

TEST(Array, RemoveWithPredicate)
{
	static const int ExpectedResults[]{
		1,3,4
	};

	Baroque::Array<TestComplexType> array;

	array.Add(1);
	array.Add(2);
	array.Add(3);
	array.Add(2);
	array.Add(4);

	TestComplexType::Reset();

	auto removed = array.RemoveByPredicate([](const TestComplexType& item) {
		return item.DummyValue == 2;
	});

	EXPECT_EQ(TestComplexType::DtorCount, 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 3);

	EXPECT_EQ(removed, 2);
	EXPECT_EQ(array.Size(), 3);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i].DummyValue, ExpectedResults[i]);
	}
}

TEST(Array, RemoveAt)
{
	static const int ExpectedResults[]{
		1,3,4
	};

	Baroque::Array<TestComplexType> array;

	array.Add(1);
	array.Add(2);
	array.Add(3);
	array.Add(4);

	TestComplexType::Reset();

	array.RemoveAt(1);

	EXPECT_EQ(TestComplexType::DtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 2);

	EXPECT_EQ(array.Size(), 3);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i].DummyValue, ExpectedResults[i]);
	}
}

TEST(Array, RemoveAtFront)
{
	static const int ExpectedResults[]{
		2,3,4
	};

	Baroque::Array<TestComplexType> array;

	array.Add(1);
	array.Add(2);
	array.Add(3);
	array.Add(4);

	TestComplexType::Reset();

	array.RemoveAt(0);

	EXPECT_EQ(TestComplexType::DtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 3);

	EXPECT_EQ(array.Size(), 3);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i].DummyValue, ExpectedResults[i]);
	}
}

TEST(Array, RemoveAtLast)
{
	static const int ExpectedResults[]{
		1,2,3
	};

	Baroque::Array<TestComplexType> array;

	array.Add(1);
	array.Add(2);
	array.Add(3);
	array.Add(4);

	TestComplexType::Reset();

	array.RemoveAt(array.Size()-1);

	EXPECT_EQ(TestComplexType::DtorCount, 1);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);

	EXPECT_EQ(array.Size(), 3);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i].DummyValue, ExpectedResults[i]);
	}
}

TEST(Array, Erase)
{
	static const int ExpectedResults[]{
		1,3,4
	};

	Baroque::Array<TestComplexType> array;

	array.Add(1);
	array.Add(2);
	array.Add(3);
	array.Add(2);
	array.Add(4);

	TestComplexType::Reset();

	auto it = array.begin();
	while (it != array.end())
	{
		if (it->DummyValue == 2)
		{
			it = array.Erase(it);
		}
		else
		{
			++it;
		}
	}

	EXPECT_EQ(TestComplexType::DtorCount, 2);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 4);

	EXPECT_EQ(array.Size(), 3);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(array[i].DummyValue, ExpectedResults[i]);
	}
}

TEST(Array, ResizeUpWithNoReallocation)
{
	Baroque::Array<TestComplexType> array;

	array.Add(1);
	array.Add(2);

	auto* previousData = array.Data();

	TestComplexType::Reset();

	array.Resize(3);

	EXPECT_EQ(TestComplexType::CtorCount, 1);
	EXPECT_EQ(TestComplexType::DtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);

	EXPECT_EQ(array.Size(), 3);
	EXPECT_EQ(array.Data(), previousData);
}

TEST(Array, ResizeUpWithReallocation)
{
	Baroque::Array<TestComplexType> array;

	array.Add(1);
	array.Add(2);

	auto* previousData = array.Data();
	auto previousSize = array.Size();

	TestComplexType::Reset();

	array.Resize(16);

	EXPECT_EQ(TestComplexType::CtorCount, 16 - previousSize);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 2);

	EXPECT_EQ(array.Size(), 16);
	EXPECT_NE(array.Data(), previousData);
}

TEST(Array, ResizeDown)
{
	Baroque::Array<TestComplexType> array;

	for (int i = 0; i < 8; ++i)
	{
		array.Add(i + 1);
	}

	TestComplexType::Reset();

	array.Resize(4);

	EXPECT_EQ(TestComplexType::CtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 4);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 0);

	EXPECT_EQ(array.Size(), 4);
}

TEST(Array, Reserve)
{
	Baroque::Array<TestComplexType> array;

	for (int i = 0; i < 4; ++i)
	{
		array.Add(i + 1);
	}

	TestComplexType::Reset();

	auto previousCapacity = array.Capacity();

	array.Reserve(8);

	EXPECT_EQ(TestComplexType::CtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 0);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 4);

	EXPECT_EQ(array.Size(), 4);
	EXPECT_EQ(array.Capacity(), 8);
	EXPECT_NE(array.Capacity(), previousCapacity);
}

TEST(Array, Front)
{
	Baroque::Array<TestComplexType> array;

	array.Emplace(69);
	array.Emplace(78);

	EXPECT_EQ(array.Front().DummyValue, 69);
	
	array.Front().DummyValue = 12;

	EXPECT_EQ(array.Front().DummyValue, 12);
}

TEST(Array, Last)
{
	Baroque::Array<TestComplexType> array;

	array.Emplace(12);
	array.Emplace(34);
	array.Emplace(56);

	EXPECT_EQ(array.Last().DummyValue, 56);

	array.Last().DummyValue = 100;

	EXPECT_EQ(array.Last().DummyValue, 100);
}

TEST(Array, ShouldSupportStackSemantics)
{
	static const int ExpectedResults[] = {
		3, 2, 1
	};

	TestComplexType::Reset();

	Baroque::Array<TestComplexType> stack;

	stack.Push(TestComplexType{ 1 });
	stack.Push(TestComplexType{ 2 });
	stack.Push(TestComplexType{ 3 });

	EXPECT_EQ(TestComplexType::CtorCount, 3);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 3);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 0);

	EXPECT_EQ(stack.Size(), 3);
	EXPECT_EQ(stack.Top().DummyValue, 3);

	TestComplexType::Reset();

	for (std::size_t i = 0; i < 3; ++i)
	{
		auto poppedValue = stack.Pop();
		EXPECT_EQ(poppedValue.DummyValue, ExpectedResults[i]);
	}

	EXPECT_EQ(TestComplexType::CtorCount, 0);
	EXPECT_EQ(TestComplexType::DtorCount, 6);
	EXPECT_EQ(TestComplexType::MoveCtorCount, 3);
	EXPECT_EQ(TestComplexType::CopyCtorCount, 3);
}

TEST(Array, ToArrayView)
{
	Baroque::Array<int> array;

	array.Add(TestConstArray);

	auto view = array.ToArrayView();

	EXPECT_EQ(view.begin(), array.begin());
	EXPECT_EQ(view.end(), array.end());
	EXPECT_EQ(view.Size(), array.Size());

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(view[i], array[i]);
	}
}

TEST(Array, ToArraySpan)
{
	Baroque::Array<int> array;

	array.Add(Baroque::ArraySpan{ TestArray });

	auto span = array.ToArraySpan();

	EXPECT_EQ(span.begin(), array.begin());
	EXPECT_EQ(span.end(), array.end());
	EXPECT_EQ(span.Size(), array.Size());

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		EXPECT_EQ(span[i], array[i]);
	}
}

TEST(Array, Slice)
{
	Baroque::Array<int> full;

	for (std::size_t i = 0; i < 8; ++i)
	{
		full.Add(static_cast<int>(i + 1));
	}

	auto slice = full.Slice(1, 3);

	EXPECT_EQ(slice.Data(), full.Data() + 1);
	EXPECT_EQ(slice.Size(), 2);

	for (std::size_t i = 0; i < slice.Size(); ++i)
	{
		EXPECT_EQ(slice[i], i + 2);
	}
}

TEST(Array, Subset)
{
	Baroque::Array<int> full;

	for (std::size_t i = 0; i < 8; ++i)
	{
		full.Add(static_cast<int>(i + 1));
	}

	auto subset = full.Subset(3);

	EXPECT_EQ(subset.Data(), full.Data() + 3);
	EXPECT_EQ(subset.Size(), 8 - 3);

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

TEST(Array, OperatorEquals)
{
	Baroque::Array<int> left;
	Baroque::Array<int> right;

	EXPECT_EQ(left, right);

	left.Add(TestConstArray);
	right.Add(TestConstArray);

	EXPECT_EQ(left, right);

	right.Add(112335);

	EXPECT_FALSE(left == right);
}

TEST(Array, OperatorNotEquals)
{
	Baroque::Array<int> left;
	Baroque::Array<int> right;

	EXPECT_FALSE(left != right);

	left.Add(TestConstArray);

	EXPECT_NE(left, right);

	right.Add(Baroque::ArraySpan{ TestArray });

	EXPECT_NE(left, right);
}

TEST(SmallArray, ShouldMoveToHeapAfterStack)
{
	Baroque::SmallArray<int, 6> smallArray;

	EXPECT_EQ(smallArray.Capacity(), 6);
	EXPECT_EQ(smallArray.Size(), 0);

	for (int i = 0; i < 6; ++i)
	{
		smallArray.Add(i + 1);
	}

	EXPECT_EQ(smallArray.Size(), 6);

	std::uint8_t* startArray = reinterpret_cast<std::uint8_t*>(&smallArray);
	std::uint8_t* endArray = startArray + sizeof(smallArray);

	auto* data = reinterpret_cast<std::uint8_t*>(smallArray.Data());
	EXPECT_TRUE(data >= startArray && data <= endArray);

	smallArray.Add(7);

	data = reinterpret_cast<std::uint8_t*>(smallArray.Data());
	EXPECT_FALSE(data >= startArray && data <= endArray);
}

TEST(SmallArray, ShouldCopyCtorProperlyWhenOnStack)
{
	Baroque::SmallArray<int, 6> original;

	for (int i = 0; i < 6; ++i)
	{
		original.Add(i + 1);
	}

	std::uint8_t* originalStartArray = reinterpret_cast<std::uint8_t*>(&original);
	std::uint8_t* originalEndArray = originalStartArray + sizeof(original);
	auto* originalData = reinterpret_cast<std::uint8_t*>(original.Data());
	EXPECT_TRUE(originalData >= originalStartArray && originalData <= originalEndArray);

	Baroque::SmallArray<int, 6> copy(original);

	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());
	EXPECT_NE(copy.Data(), original.Data());

	std::uint8_t* copyStartArray = reinterpret_cast<std::uint8_t*>(&copy);
	std::uint8_t* copyEndArray = copyStartArray + sizeof(copy);
	auto* copyData = reinterpret_cast<std::uint8_t*>(copy.Data());
	EXPECT_TRUE(copyData >= copyStartArray && copyData <= copyEndArray);
}

TEST(SmallArray, ShouldCopyCtorProperlyWhenOnHeap)
{
	Baroque::SmallArray<int, 6> original;

	for (int i = 0; i < 16; ++i)
	{
		original.Add(i + 1);
	}

	std::uint8_t* originalStartArray = reinterpret_cast<std::uint8_t*>(&original);
	std::uint8_t* originalEndArray = originalStartArray + sizeof(original);
	auto* originalData = reinterpret_cast<std::uint8_t*>(original.Data());
	EXPECT_FALSE(originalData >= originalStartArray && originalData <= originalEndArray);

	Baroque::SmallArray<int, 6> copy(original);

	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());
	EXPECT_NE(copy.Data(), original.Data());

	std::uint8_t* copyStartArray = reinterpret_cast<std::uint8_t*>(&copy);
	std::uint8_t* copyEndArray = copyStartArray + sizeof(copy);
	auto* copyData = reinterpret_cast<std::uint8_t*>(copy.Data());
	EXPECT_FALSE(copyData >= copyStartArray && copyData <= copyEndArray);
}

TEST(SmallArray, SmallArrayOnStackShouldNeverMoveCtor)
{
	Baroque::SmallArray<int, 6> original;

	for (int i = 0; i < 6; ++i)
	{
		original.Add(i + 1);
	}

	std::uint8_t* originalStartArray = reinterpret_cast<std::uint8_t*>(&original);
	std::uint8_t* originalEndArray = originalStartArray + sizeof(original);
	auto* stackData = reinterpret_cast<std::uint8_t*>(original.Data());
	EXPECT_TRUE(stackData >= originalStartArray && stackData <= originalEndArray);

	auto originalSize = original.Size();
	auto originalCapacity = original.Capacity();
	auto originalData = original.Data();

	Baroque::SmallArray<int, 6> moved(std::move(original));

	EXPECT_EQ(moved.Size(), originalSize);
	EXPECT_EQ(moved.Capacity(), originalCapacity);
	EXPECT_NE(moved.Data(), originalData);

	EXPECT_EQ(original.Size(), originalSize);
	EXPECT_EQ(original.Capacity(), originalCapacity);
	EXPECT_EQ(original.Data(), originalData);
}

TEST(SmallArray, SmallArrayOnHeapShouldNeverMoveCtor)
{
	Baroque::SmallArray<int, 6> original;

	for (int i = 0; i < 16; ++i)
	{
		original.Add(i + 1);
	}

	std::uint8_t* originalStartArray = reinterpret_cast<std::uint8_t*>(&original);
	std::uint8_t* originalEndArray = originalStartArray + sizeof(original);
	auto* stackData = reinterpret_cast<std::uint8_t*>(original.Data());
	EXPECT_FALSE(stackData >= originalStartArray && stackData <= originalEndArray);

	auto originalSize = original.Size();
	auto originalCapacity = original.Capacity();
	auto originalData = original.Data();

	Baroque::SmallArray<int, 6> moved(std::move(original));

	EXPECT_EQ(moved.Size(), originalSize);
	EXPECT_EQ(moved.Capacity(), originalCapacity);
	EXPECT_NE(moved.Data(), originalData);

	EXPECT_EQ(original.Size(), originalSize);
	EXPECT_EQ(original.Capacity(), originalCapacity);
	EXPECT_EQ(original.Data(), originalData);
}

TEST(SmallArray, ShouldCopyAssignProperlyWhenOnStack)
{
	Baroque::SmallArray<int, 6> original;

	for (int i = 0; i < 6; ++i)
	{
		original.Add(i + 1);
	}

	std::uint8_t* originalStartArray = reinterpret_cast<std::uint8_t*>(&original);
	std::uint8_t* originalEndArray = originalStartArray + sizeof(original);
	auto* originalData = reinterpret_cast<std::uint8_t*>(original.Data());
	EXPECT_TRUE(originalData >= originalStartArray && originalData <= originalEndArray);

	Baroque::SmallArray<int, 6> copy;
	
	copy = original;

	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());
	EXPECT_NE(copy.Data(), original.Data());

	std::uint8_t* copyStartArray = reinterpret_cast<std::uint8_t*>(&copy);
	std::uint8_t* copyEndArray = copyStartArray + sizeof(copy);
	auto* copyData = reinterpret_cast<std::uint8_t*>(copy.Data());
	EXPECT_TRUE(copyData >= copyStartArray && copyData <= copyEndArray);
}

TEST(SmallArray, ShouldCopyAssignProperlyWhenOnHeap)
{
	Baroque::SmallArray<int, 6> original;

	for (int i = 0; i < 16; ++i)
	{
		original.Add(i + 1);
	}

	std::uint8_t* originalStartArray = reinterpret_cast<std::uint8_t*>(&original);
	std::uint8_t* originalEndArray = originalStartArray + sizeof(original);
	auto* originalData = reinterpret_cast<std::uint8_t*>(original.Data());
	EXPECT_FALSE(originalData >= originalStartArray && originalData <= originalEndArray);

	Baroque::SmallArray<int, 6> copy;
	
	copy = original;

	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());
	EXPECT_NE(copy.Data(), original.Data());

	std::uint8_t* copyStartArray = reinterpret_cast<std::uint8_t*>(&copy);
	std::uint8_t* copyEndArray = copyStartArray + sizeof(copy);
	auto* copyData = reinterpret_cast<std::uint8_t*>(copy.Data());
	EXPECT_FALSE(copyData >= copyStartArray && copyData <= copyEndArray);
}

TEST(SmallArray, SmallArrayOnStackShouldNeverMoveAssign)
{
	Baroque::SmallArray<int, 6> original;

	for (int i = 0; i < 6; ++i)
	{
		original.Add(i + 1);
	}

	std::uint8_t* originalStartArray = reinterpret_cast<std::uint8_t*>(&original);
	std::uint8_t* originalEndArray = originalStartArray + sizeof(original);
	auto* stackData = reinterpret_cast<std::uint8_t*>(original.Data());
	EXPECT_TRUE(stackData >= originalStartArray && stackData <= originalEndArray);

	auto originalSize = original.Size();
	auto originalCapacity = original.Capacity();
	auto originalData = original.Data();

	Baroque::SmallArray<int, 6> moved;

	moved = std::move(original);

	EXPECT_EQ(moved.Size(), originalSize);
	EXPECT_EQ(moved.Capacity(), originalCapacity);
	EXPECT_NE(moved.Data(), originalData);

	EXPECT_EQ(original.Size(), originalSize);
	EXPECT_EQ(original.Capacity(), originalCapacity);
	EXPECT_EQ(original.Data(), originalData);
}

TEST(SmallArray, SmallArrayOnHeapShouldNeverMoveAssign)
{
	Baroque::SmallArray<int, 6> original;

	for (int i = 0; i < 16; ++i)
	{
		original.Add(i + 1);
	}

	std::uint8_t* originalStartArray = reinterpret_cast<std::uint8_t*>(&original);
	std::uint8_t* originalEndArray = originalStartArray + sizeof(original);
	auto* stackData = reinterpret_cast<std::uint8_t*>(original.Data());
	EXPECT_FALSE(stackData >= originalStartArray && stackData <= originalEndArray);

	auto originalSize = original.Size();
	auto originalCapacity = original.Capacity();
	auto originalData = original.Data();

	Baroque::SmallArray<int, 6> moved;
	
	moved = std::move(original);

	EXPECT_EQ(moved.Size(), originalSize);
	EXPECT_EQ(moved.Capacity(), originalCapacity);
	EXPECT_NE(moved.Data(), originalData);

	EXPECT_EQ(original.Size(), originalSize);
	EXPECT_EQ(original.Capacity(), originalCapacity);
	EXPECT_EQ(original.Data(), originalData);
}