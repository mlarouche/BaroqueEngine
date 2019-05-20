#include <gtest/gtest.h>

#include "Core/Memory/MemorySize.h"

TEST(MemorySize, ShouldReturnProperByteCount)
{
	EXPECT_EQ(Baroque::Memory::KB{ 2 }, 2 * 1024);
	EXPECT_EQ(Baroque::Memory::MB{ 3 }, 3 * 1024 * 1024);
	EXPECT_EQ(Baroque::Memory::GB{ 1 }, 1 * 1024 * 1024 * 1024);
}

TEST(MemorySize, UserLiteralShouldWork)
{
	using namespace Baroque::Memory::Literals;

	EXPECT_EQ(2_KB, 2 * 1024);
	EXPECT_EQ(3_MB, 3 * 1024 * 1024);
	EXPECT_EQ(1_GB, 1 * 1024 * 1024 * 1024);
}