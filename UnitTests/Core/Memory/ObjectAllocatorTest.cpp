#include <gtest/gtest.h>

#include "Core/Memory/MallocAllocator.h"
#include "Core/Memory/ObjectAllocator.h"

namespace
{
	struct TestObject
	{
		TestObject()
		{
			++ConstructorCount;
		}

		~TestObject()
		{
			++DestructorCount;
		}

		std::uint8_t Data[8];

		static std::size_t ConstructorCount;
		static std::size_t DestructorCount;
	};

	struct ObjectWithParameters
	{
		ObjectWithParameters(std::int32_t value)
		: Data(value)
		{
		}

		std::int32_t Data;
	};

	std::size_t TestObject::ConstructorCount = 0;
	std::size_t TestObject::DestructorCount = 0;
}

TEST(ObjectAllocator, ShouldCallConstructorAndDestructorOfAType)
{
	TestObject::ConstructorCount = 0;
	TestObject::DestructorCount = 0;

	Baroque::Memory::ObjectAllocator<TestObject, Baroque::Memory::MallocAllocator> objectAllocator;

	TestObject* newObject = objectAllocator.Allocate();
	objectAllocator.Deallocate(newObject);

	EXPECT_EQ(TestObject::ConstructorCount, 1);
	EXPECT_EQ(TestObject::DestructorCount, 1);
}

TEST(ObjectAllocator, ShouldSupportConstructorWithParameters)
{
	Baroque::Memory::ObjectAllocator<ObjectWithParameters, Baroque::Memory::MallocAllocator> objectAllocator;

	ObjectWithParameters* newObject = objectAllocator.Allocate(42);
	
	EXPECT_EQ(newObject->Data, 42);

	objectAllocator.Deallocate(newObject);
}