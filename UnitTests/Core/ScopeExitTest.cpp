#include <gtest/gtest.h>

#include "Core/Utilities/ScopeExit.h"

TEST(ScopeExit, BasicCase)
{
	int dummy = 0;

	{
		BAROQUE_SCOPE_EXIT([&dummy] {
			dummy = 42;
		});

		EXPECT_EQ(dummy, 0);
	}

	EXPECT_EQ(dummy, 42);
}

TEST(ScopeExit, MultipleScopeExitInSameFunction)
{
	int intVariable = 0;
	bool boolVariable = false;

	{
		BAROQUE_SCOPE_EXIT([&intVariable] {
			intVariable = 1234;
		});

		BAROQUE_SCOPE_EXIT([&boolVariable] {
			boolVariable = true;
		});

		EXPECT_EQ(intVariable, 0);
		EXPECT_FALSE(boolVariable);
	}

	EXPECT_EQ(intVariable, 1234);
	EXPECT_TRUE(boolVariable);
}