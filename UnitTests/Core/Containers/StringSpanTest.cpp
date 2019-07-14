#include <gtest/gtest.h>

#include "Core/Containers/StringSpan.h"
#include "Core/Containers/String.h"

TEST(StringSpan, NullCtor)
{
	Baroque::StringSpan null;

	EXPECT_TRUE(null.IsNull());
	EXPECT_TRUE(null.IsEmpty());
}

TEST(StringSpan, PtrCtor)
{
	char Input[] = "Hello World!";

	Baroque::StringSpan span{ Input };

	EXPECT_FALSE(span.IsEmpty());
	EXPECT_EQ(span.Size(), 12);
	EXPECT_STREQ(span.Data(), "Hello World!");
}

TEST(StringSpan, BeginEndCtor)
{
	char Full[] = "Hello World";

	Baroque::StringSpan span(Full + 1, Full + 5);

	EXPECT_EQ(span.Size(), 4);
	EXPECT_EQ(span, "ello");
}

TEST(StringSpan, BeginPlusSizeCtor)
{
	char Full[] = "Hello World";

	Baroque::StringSpan span(Full, 5);

	EXPECT_EQ(span.Size(), 5);
	EXPECT_EQ(span, "Hello");
}

TEST(StringSpan, CopyCtor)
{
	char Input[] = "Original";

	Baroque::StringSpan original{ Input };
	Baroque::StringSpan copy(original);

	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Data(), original.Data());
	EXPECT_EQ(copy.begin(), original.begin());
	EXPECT_EQ(copy.end(), original.end());
}

TEST(StringSpan, CopyAssignment)
{
	char Input[] = "Original";

	Baroque::StringSpan original{ Input };
	Baroque::StringSpan copy;

	copy = original;

	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Data(), original.Data());
	EXPECT_EQ(copy.begin(), original.begin());
	EXPECT_EQ(copy.end(), original.end());
}

TEST(StringSpan, ByCodepoint)
{
	Baroque::Unicode::Codepoint Expected[] = {
		0x0041,
		0x00E9,
		0x96FB,
		0x1F600
	};

	char Input[] = u8"Aé電😀";
	Baroque::StringSpan span{ Input };

	std::size_t index = 0;
	for (auto codepoint : span.ByCodepoint())
	{
		EXPECT_EQ(codepoint, Expected[index]);
		++index;
	}
}

TEST(StringSpan, CodepointAt)
{
	char Input[] = u8"Aé電😀";

	Baroque::StringSpan span{ Input };

	EXPECT_EQ(span.CodepointAt(0), Baroque::Unicode::Codepoint{ 0x0041 });
	EXPECT_EQ(span.CodepointAt(1), Baroque::Unicode::Codepoint{ 0x00E9 });
	EXPECT_EQ(span.CodepointAt(2), Baroque::Unicode::Codepoint{ 0x96FB });
	EXPECT_EQ(span.CodepointAt(3), Baroque::Unicode::Codepoint{ 0x1F600 });
}

TEST(StringSpan, CodepointLength)
{
	char Input[] = u8"Aé電😀";
	Baroque::StringSpan span{ Input };

	EXPECT_EQ(span.CodepointLength(), 4);
}

TEST(StringSpan, ContainsChar)
{
	char Hello[] = "Hello!";
	char HelloWorld[] = "Hello World!";

	Baroque::StringSpan span;

	EXPECT_FALSE(span.Contains('W'));

	span = Baroque::StringSpan{ Hello };

	EXPECT_FALSE(span.Contains('W'));

	span = Baroque::StringSpan{ HelloWorld };

	EXPECT_TRUE(span.Contains('W'));
}

TEST(StringSpan, ContainsCodepoint)
{
	using namespace Baroque::Unicode;

	char Invalid[] = u8"こんにち";
	char Valid[] = u8"こんにちは！こんばんは！おはよう！";
	constexpr auto Codepoint = U'は'_cp;

	Baroque::StringSpan span{ Invalid };
	EXPECT_FALSE(span.Contains(Codepoint));

	span = Baroque::StringSpan{ Valid };
	EXPECT_TRUE(span.Contains(Codepoint));
}

TEST(StringSpan, ContainsString)
{
	char Invalid[] = "Hello!";
	char Valid[] = "Hello World!";

	Baroque::StringSpan span;
	EXPECT_FALSE(span.Contains("World"));

	span = Baroque::StringSpan{ Invalid };
	EXPECT_FALSE(span.Contains("World"));

	span = Baroque::StringSpan{ Valid };
	EXPECT_TRUE(span.Contains("World"));
}

TEST(StringSpan, ContainsStringUnicode)
{
	const char* Substring = u8"こんばんは";
	char Invalid[] = u8"こんにちは！";
	char Valid[] = u8"こんにちは！こんばんは！おはよう！";

	Baroque::StringSpan span;
	EXPECT_FALSE(span.Contains(Substring));

	span = Baroque::StringSpan{ Invalid };
	EXPECT_FALSE(span.Contains(Substring));

	span = Baroque::StringSpan{ Valid };
	EXPECT_TRUE(span.Contains(Substring));
}

TEST(StringSpan, EndsWithChar)
{
	char Invalid[] = "Hi";
	char Valid[] = "Hello World !";

	Baroque::StringSpan span;
	EXPECT_FALSE(span.EndsWith('!'));

	span = Baroque::StringSpan{ Invalid };
	EXPECT_FALSE(span.EndsWith('!'));

	span = Baroque::StringSpan{ Valid } ;
	EXPECT_TRUE(span.EndsWith("!"));
}

TEST(StringSpan, EndsWithCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr auto Codepoint = U'。'_cp;
	char Invalid[] = u8"バロックだ！";
	char Valid[] = u8"バロックです。";

	Baroque::StringSpan span;
	EXPECT_FALSE(span.EndsWith(Codepoint));

	span = Baroque::StringSpan{ Invalid };
	EXPECT_FALSE(span.EndsWith(Codepoint));

	span = Baroque::StringSpan{ Valid } ;
	EXPECT_TRUE(span.EndsWith(Codepoint));
}

TEST(StringSpan, EndsWithString)
{
	const char* ExpectedSubstring = ".txt";
	char Invalid[] = "NotAValidFilename";
	char Valid[] = "Test.txt";

	Baroque::StringSpan span;
	EXPECT_FALSE(span.EndsWith(ExpectedSubstring));

	span = Baroque::StringSpan{ Invalid };
	EXPECT_FALSE(span.EndsWith(ExpectedSubstring));

	span = Baroque::StringSpan{ Valid };
	EXPECT_TRUE(span.EndsWith(ExpectedSubstring));
}

TEST(StringSpan, EndsWithStringUnicode)
{
	const char* ExpectedSubstring = u8"です。";
	char Invalid[] = u8"バロックだ！";
	char Valid[] = u8"バロックです。";

	Baroque::StringSpan span;
	EXPECT_FALSE(span.EndsWith(ExpectedSubstring));

	span = Baroque::StringSpan{ Invalid };
	EXPECT_FALSE(span.EndsWith(ExpectedSubstring));

	span = Baroque::StringSpan{ Valid };
	EXPECT_TRUE(span.EndsWith(ExpectedSubstring));
}

TEST(StringSpan, FindChar)
{
	char Invalid[] = "Hello!";
	char Valid[] = "Hello World!";

	Baroque::StringSpan span;
	EXPECT_EQ(span.Find('W'), nullptr);

	span = Baroque::StringSpan{ Invalid };
	EXPECT_EQ(span.Find('W'), nullptr);

	span = Baroque::StringSpan{ Valid };

	auto foundIt = span.Find('W');
	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - span.Data(), 6);
	EXPECT_EQ(*foundIt, 'W');
}

TEST(StringSpan, FindCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr auto Codepoint = U'は'_cp;
	char Invalid[] = u8"こんにち";
	char Valid[] = u8"こんにちは！こんばんは！おはよう！";

	Baroque::StringSpan span;
	EXPECT_EQ(span.Find(Codepoint), nullptr);

	span = Baroque::StringSpan{ Invalid };
	EXPECT_EQ(span.Find(Codepoint), nullptr);

	span = Baroque::StringSpan{ Valid };

	auto foundIt = span.Find(Codepoint);

	EXPECT_EQ(*CodepointIterator(foundIt), Codepoint);
	EXPECT_EQ(foundIt - span.Data(), 12);
}

TEST(StringSpan, FindString)
{
	char Invalid[] = "Hello!";
	char Valid[] = "Hello World!";

	Baroque::StringSpan span;
	EXPECT_EQ(span.Find("World"), nullptr);

	span = Baroque::StringSpan{ Invalid };
	EXPECT_EQ(span.Find("World"), nullptr);

	span = Baroque::StringSpan{ Valid };

	auto foundIt = span.Find("World");
	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - span.Data(), 6);
	EXPECT_EQ(*foundIt, 'W');
}

TEST(StringSpan, FindStringUnicode)
{
	using namespace Baroque::Unicode;

	const char* Substring = u8"こんばんは";
	constexpr auto Codepoint = U'こ'_cp;
	char Invalid[] = u8"こんにちは！";
	char Valid[] = u8"こんにちは！こんばんは！おはよう！";

	Baroque::StringSpan span;
	EXPECT_EQ(span.Find(Substring), nullptr);

	span = Baroque::StringSpan{ Invalid };
	EXPECT_EQ(span.Find(Substring), nullptr);

	span = Baroque::StringSpan{ Valid };

	auto foundIt = span.Find(Substring);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - span.Data(), 18);
	EXPECT_EQ(*CodepointIterator(foundIt), Codepoint);
}

TEST(StringSpan, IndexOfChar)
{
	char Invalid[] = "Hello!";
	char Valid[] = "Hello World!";

	Baroque::StringSpan span;
	EXPECT_EQ(span.IndexOf('W'), span.Size());

	span = Baroque::StringSpan{ Invalid };
	EXPECT_EQ(span.IndexOf('W'), span.Size());

	span = Baroque::StringSpan{ Valid };
	EXPECT_EQ(span.IndexOf('W'), 6);
}

TEST(StringSpan, IndexOfCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr auto Codepoint = U'は'_cp;
	char Invalid[] = u8"こんにち";
	char Valid[] = u8"こんにちは！こんばんは！おはよう！";

	Baroque::StringSpan span;
	EXPECT_EQ(span.IndexOf(Codepoint), span.Size());

	span = Baroque::StringSpan{ Invalid };
	EXPECT_EQ(span.IndexOf(Codepoint), span.Size());

	span = Baroque::StringSpan{ Valid };
	EXPECT_EQ(span.IndexOf(Codepoint), 12);
}

TEST(StringSpan, IndexOfString)
{
	char Invalid[] = "Hello!";
	char Valid[] = "Hello World!";

	Baroque::StringSpan span;
	EXPECT_EQ(span.IndexOf("World"), span.Size());

	span = Baroque::StringSpan{ Invalid };
	EXPECT_EQ(span.IndexOf("World"), span.Size());

	span = Baroque::StringSpan{ Valid };
	EXPECT_EQ(span.IndexOf("World"), 6);
}

TEST(StringSpan, IndexOfStringUnicode)
{
	const char* Substring = u8"こんばんは";
	char Invalid[] = u8"こんにちは！";
	char Valid[] = u8"こんにちは！こんばんは！おはよう！";

	Baroque::StringSpan span;
	EXPECT_EQ(span.IndexOf(Substring), span.Size());

	span = Baroque::StringSpan{ Invalid };
	EXPECT_EQ(span.IndexOf(Substring), span.Size());

	span = Baroque::StringSpan{ Valid };
	EXPECT_EQ(span.IndexOf(Substring), 18);
}

TEST(StringSpan, LastIndexOfChar)
{
	char Invalid[] = "NoExtension";
	char Valid[] = "Sample.tar.gz";

	Baroque::StringSpan span;
	EXPECT_EQ(span.LastIndexOf('.'), span.Size());

	span = Baroque::StringSpan{ Invalid };
	EXPECT_EQ(span.LastIndexOf('.'), span.Size());

	span = Baroque::StringSpan{ Valid };
	EXPECT_EQ(span.LastIndexOf('.'), 10);
}

TEST(StringSpan, LastIndexOfCodepoint)
{
	using namespace Baroque::Unicode;

	char Invalid[] = u8"SomeFilenameデータファイル";
	char Valid[] = u8"Parent¥Folder¥SomeFilename.dat";

	constexpr const auto Codepoint = U'¥'_cp;

	Baroque::StringSpan span;

	EXPECT_EQ(span.LastIndexOf(Codepoint), span.Size());

	span = Baroque::StringSpan{ Invalid };

	EXPECT_EQ(span.LastIndexOf(Codepoint), span.Size());

	span = Baroque::StringSpan{ Valid };

	EXPECT_EQ(span.LastIndexOf(Codepoint), 14);
}

TEST(StringSpan, LastIndexOfString)
{
	char Invalid[] = "DoesNotContains";
	char Valid[] = "Windows 95, Windows 10";

	Baroque::StringSpan span;

	EXPECT_EQ(span.LastIndexOf("Windows"), span.Size());

	span = Baroque::StringSpan{ Invalid };

	EXPECT_EQ(span.LastIndexOf("Windows"), span.Size());

	span = Baroque::StringSpan{ Valid };

	EXPECT_EQ(span.LastIndexOf("Windows"), 12);
}

TEST(StringSpan, LastIndexOfStringUnicode)
{
	const char* Substring = u8"大きい";
	char Invalid[] = u8"小さい";
	char Valid[] = u8"大きい、小さい、大きい";

	Baroque::StringSpan span;

	EXPECT_EQ(span.LastIndexOf(Substring), span.Size());

	span = Baroque::StringSpan{ Invalid };

	EXPECT_EQ(span.LastIndexOf(Substring), span.Size());

	span = Baroque::StringSpan{ Valid };

	EXPECT_EQ(span.LastIndexOf(Substring), 24);
}

TEST(StringSpan, ReverseFindChar)
{
	char Invalid[] = "NoExtension";
	char Valid[] = "Sample.tar.gz";

	Baroque::StringSpan span;

	EXPECT_EQ(span.ReverseFind('.'), nullptr);

	span = Baroque::StringSpan{ Invalid };

	EXPECT_EQ(span.ReverseFind('.'), nullptr);

	span = Baroque::StringSpan{ Valid };

	auto foundIt = span.ReverseFind('.');

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - span.Data(), 10);
	EXPECT_EQ(*foundIt, '.');
}

TEST(StringSpan, ReverseFindCodepoint)
{
	using namespace Baroque::Unicode;

	char Invalid[] = u8"SomeFilenameデータファイル";
	char Valid[] = u8"Parent¥Folder¥SomeFilename.dat";

	constexpr const auto Codepoint = U'¥'_cp;

	Baroque::StringSpan span;

	EXPECT_EQ(span.ReverseFind(Codepoint), nullptr);

	span = Baroque::StringSpan{ Invalid };

	EXPECT_EQ(span.ReverseFind(Codepoint), nullptr);

	span = Baroque::StringSpan{ Valid };

	auto foundIt = span.ReverseFind(Codepoint);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*CodepointIterator(foundIt), Codepoint);
	EXPECT_EQ(foundIt - span.Data(), 14);
}

TEST(StringSpan, ReverseFindString)
{
	char Invalid[] = "DoesNotContains";
	char Valid[] = "Windows 95, Windows 10";
	Baroque::StringSpan span;

	EXPECT_EQ(span.ReverseFind("Windows"), nullptr);

	span = Baroque::StringSpan{ Invalid };

	EXPECT_EQ(span.ReverseFind("Windows"), nullptr);

	span = Baroque::StringSpan{ Valid };

	auto foundIt = span.ReverseFind("Windows");

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*foundIt, 'W');
	EXPECT_EQ(foundIt - span.Data(), 12);
}

TEST(StringSpan, ReverseFindStringUnicode)
{
	using namespace Baroque::Unicode;

	char Invalid[] = u8"小さい";
	char Valid[] = u8"大きい、小さい、大きい";

	const char* Substring = u8"大きい";
	constexpr auto FirstCodepoint = U'大'_cp;

	Baroque::StringSpan span;

	EXPECT_EQ(span.ReverseFind(Substring), nullptr);

	span = Baroque::StringSpan{ Invalid };

	EXPECT_EQ(span.ReverseFind(Substring), nullptr);

	span = Baroque::StringSpan{ Valid };

	auto foundIt = span.ReverseFind(Substring);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*(CodepointIterator(foundIt)), FirstCodepoint);
	EXPECT_EQ(foundIt - span.Data(), 24);
}

TEST(StringSpan, Slice)
{
	char Input[] = "0123456789";
	Baroque::StringSpan full{ Input };

	auto slice = full.Slice(1, 3);

	EXPECT_EQ(slice.Size(), 2);
	EXPECT_EQ(slice, "12");
}

TEST(StringSpan, StartsWithChar)
{
	char Input[] = "Hello World!";

	Baroque::StringSpan span;

	EXPECT_FALSE(span.StartsWith('H'));

	span = Baroque::StringSpan{ Input };

	EXPECT_TRUE(span.StartsWith('H'));
	EXPECT_FALSE(span.StartsWith('W'));
}

TEST(StringSpan, StartsWithCodepoint)
{
	using namespace Baroque::Unicode;

	char Input[] = u8"こんにちは！";

	constexpr auto CorrectCodepoint = U'こ'_cp;
	constexpr auto BadCodepoint = U'は'_cp;

	Baroque::StringSpan span;

	EXPECT_FALSE(span.StartsWith(CorrectCodepoint));

	span = Baroque::StringSpan{ Input };

	EXPECT_TRUE(span.StartsWith(CorrectCodepoint));
	EXPECT_FALSE(span.StartsWith(BadCodepoint));
}

TEST(StringSpan, StartsWithString)
{
	char Input[] = "Hello World!";

	Baroque::StringSpan span;

	EXPECT_FALSE(span.StartsWith("Hello"));

	span = Baroque::StringSpan{ Input };

	EXPECT_TRUE(span.StartsWith("Hello"));
	EXPECT_FALSE(span.StartsWith("World"));
}

TEST(StringSpan, StartsWithStringUnicode)
{
	const char* ExpectedSubstring = u8"こんにちは";
	const char* NotExpectedSubstring = u8"こんばんは";
	char Input[] = u8"こんにちは！　よろしくお願いします！";

	Baroque::StringSpan span;

	EXPECT_FALSE(span.StartsWith(ExpectedSubstring));

	span = Baroque::StringSpan{ Input };

	EXPECT_TRUE(span.StartsWith(ExpectedSubstring));
	EXPECT_FALSE(span.StartsWith(NotExpectedSubstring));
}

TEST(StringSpan, SubstringWithStart)
{
	char Input[] = "C:\\First\\Second\\Test.txt";
	Baroque::StringSpan full{ Input };

	auto fileName = full.Substring(full.LastIndexOf('\\') + 1);

	EXPECT_EQ(fileName.Size(), 8);
	EXPECT_EQ(fileName, "Test.txt");
}

TEST(StringSpan, SubstringWithStartAndCount)
{
	char Input[] = "C:\\First\\Second\\Test.txt";
	Baroque::StringSpan full{ Input };

	auto fileName = full.Substring(full.LastIndexOf('\\') + 1, 4);

	EXPECT_EQ(fileName.Size(), 4);
	EXPECT_EQ(fileName, "Test");
}

TEST(StringSpan, TrimBegin)
{
	Baroque::String full{ "\xE2\x80\x80 \n\n \t \r\r\r   Trim Test" };

	for (char32_t it = 0x0009; it <= 0x000D; ++it)
	{
		full.Insert(0, Baroque::Unicode::Codepoint{ it });
	}
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x0085 });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x00A0 });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x1680 });
	for (char32_t it = 0x2000; it <= 0x200A; ++it)
	{
		full.Insert(0, Baroque::Unicode::Codepoint{ it });
	}
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x2028 });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x2029 });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x202F });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x205F });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x3000 });

	auto span = full.ToStringSpan();
	auto trimmed = span.TrimBegin();

	EXPECT_EQ(trimmed.Size(), 9);
	EXPECT_EQ(trimmed, "Trim Test");
}

TEST(StringSpan, TrimEnd)
{
	Baroque::String full{ "Trim Test\xE2\x80\x80 \n\n \t \r\r\r   " };

	for (char32_t it = 0x0009; it <= 0x000D; ++it)
	{
		full.Append(Baroque::Unicode::Codepoint{ it });
	}
	full.Append(Baroque::Unicode::Codepoint{ 0x0085 });
	full.Append(Baroque::Unicode::Codepoint{ 0x00A0 });
	full.Append(Baroque::Unicode::Codepoint{ 0x1680 });
	for (char32_t it = 0x2000; it <= 0x200A; ++it)
	{
		full.Append(Baroque::Unicode::Codepoint{ it });
	}
	full.Append(Baroque::Unicode::Codepoint{ 0x2028 });
	full.Append(Baroque::Unicode::Codepoint{ 0x2029 });
	full.Append(Baroque::Unicode::Codepoint{ 0x202F });
	full.Append(Baroque::Unicode::Codepoint{ 0x205F });
	full.Append(Baroque::Unicode::Codepoint{ 0x3000 });

	auto span = full.ToStringSpan();
	auto trimmed = span.TrimEnd();

	EXPECT_EQ(trimmed.Size(), 9);
	EXPECT_EQ(trimmed, "Trim Test");
}

TEST(StringSpan, Trim)
{
	Baroque::String full{ "\xE2\x80\x80 \n\n \t \r\r\r   Trim Test\xE2\x80\x80 \n\n \t \r\r\r   " };

	for (char32_t it = 0x0009; it <= 0x000D; ++it)
	{
		full.Insert(0, Baroque::Unicode::Codepoint{ it });
		full.Append(Baroque::Unicode::Codepoint{ it });
	}
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x0085 });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x00A0 });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x1680 });
	full.Append(Baroque::Unicode::Codepoint{ 0x0085 });
	full.Append(Baroque::Unicode::Codepoint{ 0x00A0 });
	full.Append(Baroque::Unicode::Codepoint{ 0x1680 });
	for (char32_t it = 0x2000; it <= 0x200A; ++it)
	{
		full.Insert(0, Baroque::Unicode::Codepoint{ it });
		full.Append(Baroque::Unicode::Codepoint{ it });
	}
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x2028 });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x2029 });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x202F });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x205F });
	full.Insert(0, Baroque::Unicode::Codepoint{ 0x3000 });
	full.Append(Baroque::Unicode::Codepoint{ 0x2028 });
	full.Append(Baroque::Unicode::Codepoint{ 0x2029 });
	full.Append(Baroque::Unicode::Codepoint{ 0x202F });
	full.Append(Baroque::Unicode::Codepoint{ 0x205F });
	full.Append(Baroque::Unicode::Codepoint{ 0x3000 });

	auto span = full.ToStringSpan();
	auto trimmed = span.Trim();

	EXPECT_EQ(trimmed.Size(), 9);
	EXPECT_EQ(trimmed, "Trim Test");
}

TEST(StringSpan, ToString)
{
	char Input[] = "Hello World";

	Baroque::StringSpan span{ Input };

	auto toString = span.ToString();

	EXPECT_EQ(span.Size(), toString.Size());
	EXPECT_STREQ(span.Data(), toString.Data());
	EXPECT_NE(span.Data(), toString.Data());
}

TEST(StringSpan, ToStringType)
{
	char Input[] = "Hello World";

	Baroque::StringSpan span{ Input };

	auto toSmallString = span.ToStringType<Baroque::SmallString<48>>();

	EXPECT_EQ(span.Size(), toSmallString.Size());
	EXPECT_STREQ(span.Data(), toSmallString.Data());
	EXPECT_NE(span.Data(), toSmallString.Data());
}

TEST(StringSpan, ModifyStringSpanShouldWork)
{
	char Input[] = "Hello World";

	Baroque::StringSpan span{ Input };

	span[0] = 'W';

	EXPECT_EQ(span, "Wello World");
}
