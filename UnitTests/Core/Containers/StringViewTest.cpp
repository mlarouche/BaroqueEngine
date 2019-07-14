#include <gtest/gtest.h>

#include "Core/Containers/StringView.h"
#include "Core/Containers/String.h"

TEST(StringView, NullCtor)
{
	Baroque::StringView null;

	EXPECT_TRUE(null.IsNull());
	EXPECT_TRUE(null.IsEmpty());
}

TEST(StringView, EmptyCtor)
{
	Baroque::StringView empty{ "" };

	EXPECT_FALSE(empty.IsNull());
	EXPECT_TRUE(empty.IsEmpty());
}

TEST(StringView, ConstPtrCtor)
{
	Baroque::StringView view{ "Hello World!" };

	EXPECT_FALSE(view.IsEmpty());
	EXPECT_EQ(view.Size(), 12);
	EXPECT_STREQ(view.Data(), "Hello World!");
}

TEST(StringView, BeginEndCtor)
{
	const char* Full = "Hello World";

	Baroque::StringView view(Full + 1, Full + 5);

	EXPECT_EQ(view.Size(), 4);
	EXPECT_EQ(view, "ello");
}

TEST(StringView, BeginPlusSizeCtor)
{
	const char* Full = "Hello World";

	Baroque::StringView view(Full, 5);

	EXPECT_EQ(view.Size(), 5);
	EXPECT_EQ(view, "Hello");
}

TEST(StringView, StringSpanCtor)
{
	Baroque::String string{ "Test" };

	auto span = string.ToStringSpan();

	Baroque::StringView view(span);

	EXPECT_EQ(view.Size(), span.Size());
	EXPECT_EQ(view.Data(), span.Data());
	EXPECT_EQ(view.begin(), span.begin());
	EXPECT_EQ(view.end(), span.end());
}

TEST(StringView, StringCtor)
{
	Baroque::String full{ "Testing" };

	Baroque::StringView view(full);

	EXPECT_EQ(view.Size(), full.Size());
	EXPECT_EQ(view.Data(), full.Data());
	EXPECT_EQ(view.begin(), full.begin());
	EXPECT_EQ(view.end(), full.end());
}

TEST(StringView, SmallStringCtor)
{
	Baroque::SmallString<32> full{ "Testing" };

	Baroque::StringView view(full);

	EXPECT_EQ(view.Size(), full.Size());
	EXPECT_EQ(view.Data(), full.Data());
	EXPECT_EQ(view.begin(), full.begin());
	EXPECT_EQ(view.end(), full.end());
}

TEST(StringView, CopyCtor)
{
	Baroque::StringView original{ "Original" };
	Baroque::StringView copy(original);

	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Data(), original.Data());
	EXPECT_EQ(copy.begin(), original.begin());
	EXPECT_EQ(copy.end(), original.end());
}

TEST(StringView, CopyAssignment)
{
	Baroque::StringView original{ "Original" };
	Baroque::StringView copy;

	copy = original;

	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Data(), original.Data());
	EXPECT_EQ(copy.begin(), original.begin());
	EXPECT_EQ(copy.end(), original.end());
}

TEST(StringView, ByCodepoint)
{
	Baroque::Unicode::Codepoint Expected[] = {
		0x0041,
		0x00E9,
		0x96FB,
		0x1F600
	};

	Baroque::StringView view{ u8"Aé電😀" };

	std::size_t index = 0;
	for (auto codepoint : view.ByCodepoint())
	{
		EXPECT_EQ(codepoint, Expected[index]);
		++index;
	}
}

TEST(StringView, CodepointAt)
{
	Baroque::StringView view{ u8"Aé電😀" };

	EXPECT_EQ(view.CodepointAt(0), Baroque::Unicode::Codepoint{ 0x0041 });
	EXPECT_EQ(view.CodepointAt(1), Baroque::Unicode::Codepoint{ 0x00E9 });
	EXPECT_EQ(view.CodepointAt(2), Baroque::Unicode::Codepoint{ 0x96FB });
	EXPECT_EQ(view.CodepointAt(3), Baroque::Unicode::Codepoint{ 0x1F600 });
}

TEST(StringView, CodepointLength)
{
	Baroque::StringView view{ u8"Aé電😀" };

	EXPECT_EQ(view.CodepointLength(), 4);
}

TEST(StringView, ContainsChar)
{
	Baroque::StringView view;

	EXPECT_FALSE(view.Contains('W'));

	view = "Hello!";

	EXPECT_FALSE(view.Contains('W'));

	view = "Hello World!";

	EXPECT_TRUE(view.Contains('W'));
}

TEST(StringView, ContainsCodepoint)
{
	using namespace Baroque::Unicode;

	Baroque::StringView view{ u8"こんにち" };

	auto codepoint = U'は'_cp;

	EXPECT_FALSE(view.Contains(codepoint));

	view = u8"こんにちは！こんばんは！おはよう！";

	EXPECT_TRUE(view.Contains(codepoint));
}

TEST(StringView, ContainsString)
{
	Baroque::StringView view;

	EXPECT_FALSE(view.Contains("World"));

	view = "Hello!";

	EXPECT_FALSE(view.Contains("World"));

	view = "Hello World!";

	EXPECT_TRUE(view.Contains("World"));
}

TEST(StringView, ContainsStringUnicode)
{
	const char* Substring = u8"こんばんは";

	Baroque::StringView view;

	EXPECT_FALSE(view.Contains(Substring));

	view = u8"こんにちは！";

	EXPECT_FALSE(view.Contains(Substring));

	view = u8"こんにちは！こんばんは！おはよう！";

	EXPECT_TRUE(view.Contains(Substring));
}

TEST(StringView, EndsWithChar)
{
	Baroque::StringView view;

	EXPECT_FALSE(view.EndsWith('!'));

	view = "Hi";

	EXPECT_FALSE(view.EndsWith('!'));

	view = "Hello World !";

	EXPECT_TRUE(view.EndsWith("!"));
}

TEST(StringView, EndsWithCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr auto ExpectedCodepoint = U'。'_cp;

	Baroque::StringView view;

	EXPECT_FALSE(view.EndsWith(ExpectedCodepoint));

	view = u8"バロックだ！";

	EXPECT_FALSE(view.EndsWith(ExpectedCodepoint));

	view = u8"バロックです。";

	EXPECT_TRUE(view.EndsWith(ExpectedCodepoint));
}

TEST(StringView, EndsWithString)
{
	const char* ExpectedSubstring = ".txt";

	Baroque::StringView view;

	EXPECT_FALSE(view.EndsWith(ExpectedSubstring));

	view = "NotAValidFilename";

	EXPECT_FALSE(view.EndsWith(ExpectedSubstring));

	view = "Test.txt";

	EXPECT_TRUE(view.EndsWith(ExpectedSubstring));
}

TEST(StringView, EndsWithStringUnicode)
{
	const char* ExpectedSubstring = u8"です。";

	Baroque::StringView view;

	EXPECT_FALSE(view.EndsWith(ExpectedSubstring));

	view = u8"バロックだ！";

	EXPECT_FALSE(view.EndsWith(ExpectedSubstring));

	view = u8"バロックです。";

	EXPECT_TRUE(view.EndsWith(ExpectedSubstring));
}

TEST(StringView, FindChar)
{
	Baroque::StringView view;

	EXPECT_EQ(view.Find('W'), nullptr);

	view = "Hello!";

	EXPECT_EQ(view.Find('W'), nullptr);

	view = "Hello World!";

	auto foundIt = view.Find('W');
	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - view.Data(), 6);
	EXPECT_EQ(*foundIt, 'W');
}

TEST(StringView, FindCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr auto Codepoint = U'は'_cp;

	Baroque::StringView view{ u8"こんにち" };

	EXPECT_EQ(view.Find(Codepoint), nullptr);

	view = u8"こんにちは！こんばんは！おはよう！";

	auto foundIt = view.Find(Codepoint);

	EXPECT_EQ(*CodepointIterator(foundIt), Codepoint);
	EXPECT_EQ(foundIt - view.Data(), 12);
}

TEST(StringView, FindString)
{
	Baroque::StringView view;

	EXPECT_EQ(view.Find("World"), nullptr);

	view = "Hello!";

	EXPECT_EQ(view.Find("World"), nullptr);

	view = "Hello World!";

	auto foundIt = view.Find("World");
	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - view.Data(), 6);
	EXPECT_EQ(*foundIt, 'W');
}

TEST(StringView, FindStringUnicode)
{
	using namespace Baroque::Unicode;

	const char* Substring = u8"こんばんは";
	constexpr auto ExpectedCodepoint = U'こ'_cp;

	Baroque::StringView view;

	EXPECT_EQ(view.Find(Substring), nullptr);

	view = u8"こんにちは！";

	EXPECT_EQ(view.Find(Substring), nullptr);

	view = u8"こんにちは！こんばんは！おはよう！";

	auto foundIt = view.Find(Substring);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - view.Data(), 18);
	EXPECT_EQ(*CodepointIterator(foundIt), ExpectedCodepoint);
}

TEST(StringView, IndexOfChar)
{
	Baroque::StringView view;

	EXPECT_EQ(view.IndexOf('W'), view.Size());

	view = "Hello!";

	EXPECT_EQ(view.IndexOf('W'), view.Size());

	view = "Hello World!";

	EXPECT_EQ(view.IndexOf('W'), 6);
}

TEST(StringView, IndexOfCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr auto Codepoint = U'は'_cp;

	Baroque::StringView view{ u8"こんにち" };

	EXPECT_EQ(view.IndexOf(Codepoint), view.Size());

	view = u8"こんにちは！こんばんは！おはよう！";

	EXPECT_EQ(view.IndexOf(Codepoint), 12);
}

TEST(StringView, IndexOfString)
{
	Baroque::StringView view;

	EXPECT_EQ(view.IndexOf("World"), view.Size());

	view = "Hello!";

	EXPECT_EQ(view.IndexOf("World"), view.Size());

	view = "Hello World!";

	EXPECT_EQ(view.IndexOf("World"), 6);
}

TEST(StringView, IndexOfStringUnicode)
{
	const char* Substring = u8"こんばんは";

	Baroque::StringView view;

	EXPECT_EQ(view.IndexOf(Substring), view.Size());

	view = u8"こんにちは！";

	EXPECT_EQ(view.IndexOf(Substring), view.Size());

	view = u8"こんにちは！こんばんは！おはよう！";

	EXPECT_EQ(view.IndexOf(Substring), 18);
}

TEST(StringView, LastIndexOfChar)
{
	Baroque::StringView view;

	EXPECT_EQ(view.LastIndexOf('.'), view.Size());

	view = "NoExtension";

	EXPECT_EQ(view.LastIndexOf('.'), view.Size());

	view = "Sample.tar.gz";

	EXPECT_EQ(view.LastIndexOf('.'), 10);
}

TEST(StringView, LastIndexOfCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr const auto Codepoint = U'¥'_cp;

	Baroque::StringView view;

	EXPECT_EQ(view.LastIndexOf(Codepoint), view.Size());

	view = u8"SomeFilenameデータファイル";

	EXPECT_EQ(view.LastIndexOf(Codepoint), view.Size());

	view = u8"Parent¥Folder¥SomeFilename.dat";

	EXPECT_EQ(view.LastIndexOf(Codepoint), 14);
}

TEST(StringView, LastIndexOfString)
{
	Baroque::StringView view;

	EXPECT_EQ(view.LastIndexOf("Windows"), view.Size());

	view = "DoesNotContains";

	EXPECT_EQ(view.LastIndexOf("Windows"), view.Size());

	view = "Windows 95, Windows 10";

	EXPECT_EQ(view.LastIndexOf("Windows"), 12);
}

TEST(StringView, LastIndexOfStringUnicode)
{
	const char* Substring = u8"大きい";

	Baroque::StringView view;

	EXPECT_EQ(view.LastIndexOf(Substring), view.Size());

	view = u8"小さい";

	EXPECT_EQ(view.LastIndexOf(Substring), view.Size());

	view = u8"大きい、小さい、大きい";

	EXPECT_EQ(view.LastIndexOf(Substring), 24);
}

TEST(StringView, ReverseFindChar)
{
	Baroque::StringView view;

	EXPECT_EQ(view.ReverseFind('.'), nullptr);

	view = "NoExtension";

	EXPECT_EQ(view.ReverseFind('.'), nullptr);

	view = "Sample.tar.gz";

	auto foundIt = view.ReverseFind('.');

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - view.Data(), 10);
	EXPECT_EQ(*foundIt, '.');
}

TEST(StringView, ReverseFindCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr const auto Codepoint = U'¥'_cp;

	Baroque::StringView view;

	EXPECT_EQ(view.ReverseFind(Codepoint), nullptr);

	view = u8"SomeFilenameデータファイル";

	EXPECT_EQ(view.ReverseFind(Codepoint), nullptr);

	view = u8"Parent¥Folder¥SomeFilename.dat";

	auto foundIt = view.ReverseFind(Codepoint);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*CodepointIterator(foundIt), Codepoint);
	EXPECT_EQ(foundIt - view.Data(), 14);
}

TEST(StringView, ReverseFindString)
{
	Baroque::StringView view;

	EXPECT_EQ(view.ReverseFind("Windows"), nullptr);

	view = "DoesNotContains";

	EXPECT_EQ(view.ReverseFind("Windows"), nullptr);

	view = "Windows 95, Windows 10";

	auto foundIt = view.ReverseFind("Windows");

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*foundIt, 'W');
	EXPECT_EQ(foundIt - view.Data(), 12);
}

TEST(StringView, ReverseFindStringUnicode)
{
	using namespace Baroque::Unicode;

	const char* Substring = u8"大きい";
	constexpr auto FirstCodepoint = U'大'_cp;

	Baroque::StringView view;

	EXPECT_EQ(view.ReverseFind(Substring), nullptr);

	view = u8"小さい";

	EXPECT_EQ(view.ReverseFind(Substring), nullptr);

	view = u8"大きい、小さい、大きい";

	auto foundIt = view.ReverseFind(Substring);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*(CodepointIterator(foundIt)), FirstCodepoint);
	EXPECT_EQ(foundIt - view.Data(), 24);
}

TEST(StringView, Slice)
{
	Baroque::StringView full{ "0123456789" };

	auto slice = full.Slice(1, 3);

	EXPECT_EQ(slice.Size(), 2);
	EXPECT_EQ(slice, "12");
}

TEST(StringView, StartsWithChar)
{
	Baroque::StringView view;

	EXPECT_FALSE(view.StartsWith('H'));

	view = "Hello World!";

	EXPECT_TRUE(view.StartsWith('H'));
	EXPECT_FALSE(view.StartsWith('W'));
}

TEST(StringView, StartsWithCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr auto CorrectCodepoint = U'こ'_cp;
	constexpr auto BadCodepoint = U'は'_cp;

	Baroque::StringView view;

	EXPECT_FALSE(view.StartsWith(CorrectCodepoint));

	view = u8"こんにちは！";

	EXPECT_TRUE(view.StartsWith(CorrectCodepoint));
	EXPECT_FALSE(view.StartsWith(BadCodepoint));
}

TEST(StringView, StartsWithString)
{
	Baroque::StringView view;

	EXPECT_FALSE(view.StartsWith("Hello"));

	view = "Hello World!";

	EXPECT_TRUE(view.StartsWith("Hello"));
	EXPECT_FALSE(view.StartsWith("World"));
}

TEST(StringView, StartsWithStringUnicode)
{
	const char* ExpectedSubstring = u8"こんにちは";
	const char* NotExpectedSubstring = u8"こんばんは";

	Baroque::StringView view;

	EXPECT_FALSE(view.StartsWith(ExpectedSubstring));

	view = u8"こんにちは！　よろしくお願いします！";

	EXPECT_TRUE(view.StartsWith(ExpectedSubstring));
	EXPECT_FALSE(view.StartsWith(NotExpectedSubstring));
}

TEST(StringView, SubstringWithStart)
{
	Baroque::StringView full{ "C:\\First\\Second\\Test.txt" };

	auto fileName = full.Substring(full.LastIndexOf('\\') + 1);

	EXPECT_EQ(fileName.Size(), 8);
	EXPECT_EQ(fileName, "Test.txt");
}

TEST(StringView, SubstringWithStartAndCount)
{
	Baroque::StringView full{ "C:\\First\\Second\\Test.txt" };

	auto fileName = full.Substring(full.LastIndexOf('\\') + 1, 4);

	EXPECT_EQ(fileName.Size(), 4);
	EXPECT_EQ(fileName, "Test");
}

TEST(StringView, TrimBegin)
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

	auto view = full.ToStringView();
	auto trimmed = view.TrimBegin();

	EXPECT_EQ(trimmed.Size(), 9);
	EXPECT_EQ(trimmed, "Trim Test");
}

TEST(StringView, TrimEnd)
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

	auto view = full.ToStringView();
	auto trimmed = view.TrimEnd();

	EXPECT_EQ(trimmed.Size(), 9);
	EXPECT_EQ(trimmed, "Trim Test");
}

TEST(StringView, Trim)
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

	auto view = full.ToStringView();
	auto trimmed = view.Trim();

	EXPECT_EQ(trimmed.Size(), 9);
	EXPECT_EQ(trimmed, "Trim Test");
}

TEST(StringView, ToString)
{
	Baroque::StringView view{ "Hello World!" };

	auto toString = view.ToString();

	EXPECT_EQ(view.Size(), toString.Size());
	EXPECT_STREQ(view.Data(), toString.Data());
	EXPECT_NE(view.Data(), toString.Data());
	EXPECT_EQ(toString, view);
}

TEST(StringView, ToStringType)
{
	Baroque::StringView view{ "Hello World!" };

	auto toSmallString = view.ToStringType<Baroque::SmallString<48>>();

	EXPECT_EQ(view.Size(), toSmallString.Size());
	EXPECT_STREQ(view.Data(), toSmallString.Data());
	EXPECT_NE(view.Data(), toSmallString.Data());
	EXPECT_EQ(view, toSmallString);
}

TEST(StringView, OperatorEquals)
{
	Baroque::StringView leftString("LeftString");
	Baroque::StringView rightString("RightString");
	Baroque::StringView similarLeftString("LeftString");
	Baroque::StringView sameSizeButDifferentContent("0123456789");

	EXPECT_FALSE(leftString == rightString);
	EXPECT_TRUE(leftString == leftString);
	EXPECT_TRUE(leftString == similarLeftString);
	EXPECT_FALSE(leftString == sameSizeButDifferentContent);

	EXPECT_FALSE(leftString == "RightString");
	EXPECT_TRUE(leftString == "LeftString");
	EXPECT_FALSE("LeftString" == rightString);
	EXPECT_TRUE("LeftString" == leftString);
}

TEST(StringView, OperatorNotEquals)
{
	Baroque::StringView leftString("LeftString");
	Baroque::StringView rightString("RightString");
	Baroque::StringView similarLeftString("LeftString");
	Baroque::StringView sameSizeButDifferentContent("0123456789");

	EXPECT_TRUE(leftString != rightString);
	EXPECT_FALSE(leftString != leftString);
	EXPECT_FALSE(leftString != similarLeftString);
	EXPECT_TRUE(leftString != sameSizeButDifferentContent);

	EXPECT_TRUE(leftString != "RightString");
	EXPECT_FALSE(leftString != "LeftString");
	EXPECT_TRUE("LeftString" != rightString);
	EXPECT_FALSE("LeftString" != leftString);
}

TEST(StringView, OperatorLess)
{
	Baroque::StringView a{ "a" };
	Baroque::StringView b{ "b" };
	Baroque::StringView bb{ "bb" };

	EXPECT_TRUE(a < b);
	EXPECT_TRUE(a < bb);
	EXPECT_FALSE(b < a);

	EXPECT_TRUE(a < "b");
	EXPECT_TRUE(a < "bb");
	EXPECT_FALSE(b < "a");

	EXPECT_TRUE("a" < b);
	EXPECT_TRUE("a" < bb);
	EXPECT_FALSE("b" < a);
}

TEST(StringView, OperatorLessOrEquals)
{
	Baroque::StringView a{ "a" };
	Baroque::StringView b{ "b" };
	Baroque::StringView bb{ "bb" };

	EXPECT_TRUE(a <= b);
	EXPECT_TRUE(a <= bb);
	EXPECT_FALSE(b <= a);
	EXPECT_TRUE(a <= a);

	EXPECT_TRUE(a <= "b");
	EXPECT_TRUE(a <= "bb");
	EXPECT_FALSE(b < "a");
	EXPECT_TRUE(a <= "a");

	EXPECT_TRUE("a" <= b);
	EXPECT_TRUE("a" <= bb);
	EXPECT_FALSE("b" <= a);
	EXPECT_TRUE("a" <= a);
}

TEST(StringView, OperatorGreater)
{
	Baroque::StringView a{ "a" };
	Baroque::StringView b{ "b" };
	Baroque::StringView bb{ "bb" };

	EXPECT_FALSE(a > b);
	EXPECT_FALSE(a > bb);
	EXPECT_TRUE(b > a);

	EXPECT_FALSE(a > "b");
	EXPECT_FALSE(a > "bb");
	EXPECT_TRUE(b > "a");

	EXPECT_FALSE("a" > b);
	EXPECT_FALSE("a" > bb);
	EXPECT_TRUE("b" > a);
}

TEST(StringView, OperatorGreaterOrEquals)
{
	Baroque::String a{ "a" };
	Baroque::String b{ "b" };
	Baroque::String bb{ "bb" };

	EXPECT_FALSE(a >= b);
	EXPECT_FALSE(a >= bb);
	EXPECT_TRUE(b >= a);
	EXPECT_TRUE(b >= b);

	EXPECT_FALSE(a > "b");
	EXPECT_FALSE(a > "bb");
	EXPECT_TRUE(b > "a");
	EXPECT_TRUE(b >= "b");

	EXPECT_FALSE("a" >= b);
	EXPECT_FALSE("a" >= bb);
	EXPECT_TRUE("b" >= a);
	EXPECT_TRUE("b" >= b);
}