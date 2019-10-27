#include <gtest/gtest.h>

#include "Core/Formatting/Format.h"
#include "Core/Formatting/Printf.h"

namespace
{
	struct TestVarFormat
	{
		template<typename...Args>
		static Baroque::String Do(Baroque::StringView format, Args&& ... args)
		{
			return Baroque::Formatting::VFormat(format, Baroque::Formatting::MakeFormatArgs(args...));
		}
	};
}

TEST(Formatting, SimpleFormat)
{
	Baroque::String formatString("The answer to life is {}");
	auto result = Baroque::Formatting::Format(formatString, 42);

	EXPECT_EQ(result, "The answer to life is 42");
}

TEST(Formatting, FormatSmallString)
{
	auto result = Baroque::Formatting::Format<Baroque::SmallString<64>>("The answer to life is {}", 42);

	EXPECT_EQ(result, "The answer to life is 42");
}

TEST(Formatting, PositionalArguments)
{
	auto result = Baroque::Formatting::Format("I'd rather be {1} than {0}.", "right", "happy");
	EXPECT_EQ(result, "I'd rather be happy than right.");
}

TEST(Formatting, FormatTo)
{
	Baroque::Formatting::MemoryBuffer buffer;
	Baroque::Formatting::FormatTo(buffer, "The answer to life is {}", 42);

	buffer.data()[buffer.size()] = '\0';
	
	EXPECT_STREQ(buffer.data(), "The answer to life is 42");
}

TEST(Formatting, ToString)
{
	auto result = Baroque::Formatting::ToString(12345);
	EXPECT_EQ(result, "12345");
}

TEST(Formatting, ToSmallString)
{
	auto result = Baroque::Formatting::ToString<Baroque::SmallString<64>>(12345);
	EXPECT_EQ(result, "12345");
}

TEST(Formatting, ToStringWithMemoryBuffer)
{
	Baroque::Formatting::MemoryBuffer buffer;
	Baroque::Formatting::FormatTo(buffer, "The answer to life is {}", 42);

	auto result = Baroque::Formatting::ToString(buffer);

	EXPECT_EQ(result, "The answer to life is 42");
}

TEST(Formatting, ToSmallStringWithMemoryBuffer)
{
	Baroque::Formatting::MemoryBuffer buffer;
	Baroque::Formatting::FormatTo(buffer, "The answer to life is {}", 42);

	auto result = Baroque::Formatting::ToString<Baroque::SmallString<64>>(buffer);

	EXPECT_EQ(result, "The answer to life is 42");
}

TEST(Formatting, NamedArguments)
{
	auto result = Baroque::Formatting::Format("Elapsed time: {s:.2f} seconds", Baroque::Formatting::Arg("s", 1.23));

	EXPECT_EQ(result, "Elapsed time: 1.23 seconds");
}

TEST(Formatting, VariantFormat)
{
	auto result = TestVarFormat::Do("Test: {}", false);

	EXPECT_EQ(result, "Test: false");
}

TEST(Formatting, FormattedSize)
{
	auto size = Baroque::Formatting::FormattedSize("The answer to life is {}", 42);
	EXPECT_EQ(size, 24);
}

TEST(Formatting, Printf)
{
	auto result = Baroque::Formatting::SPrintf("The answer to life is %d", 42);
	EXPECT_EQ(result, "The answer to life is 42");
}