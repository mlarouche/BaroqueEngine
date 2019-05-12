#include <gtest/gtest.h>

#include "Core/MemorySize.h"

TEST(MemorySize, ShouldReturnProperByteCount)
{
	EXPECT_EQ(Baroque::KB{ 2 }, 2 * 1024);
	EXPECT_EQ(Baroque::MB{ 3 }, 3 * 1024 * 1024);
	EXPECT_EQ(Baroque::GB{ 1 }, 1 * 1024 * 1024 * 1024);
}

TEST(MemorySize, UserLiteralShouldWork)
{
	using namespace Baroque;

	EXPECT_EQ(2_KB, 2 * 1024);
	EXPECT_EQ(3_MB, 3 * 1024 * 1024);
	EXPECT_EQ(1_GB, 1 * 1024 * 1024 * 1024);
}