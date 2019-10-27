#include <gtest/gtest.h>

#include "Core/Containers/String.h"
#include "UnitTests/Core/VerifyDeallocAllocator.h"

#include <cstring>

namespace
{
	const char* TestShortString = "Short";
	const char* AnotherShortString = "HelloWorld";

	const char* TestHeapString = "This is a long string that use the heap.";
	const char* AnotherHeapString = "Chrono Trigger is the best game ever.";

	char NonConstString[] = "This is a non-const string";

	using VerifyDeallocString = Baroque::StringImplementation<VerifyDealloc>;
}

TEST(String, EmptyStringShouldBeProperlyInitialized)
{
	Baroque::String string;

	EXPECT_TRUE(string.IsShortString());
	EXPECT_NE(string.Data(), nullptr);
	EXPECT_EQ(string.Size(), 0);
	EXPECT_EQ(string.Capacity(), sizeof(string) - 1);
	EXPECT_TRUE(string.IsEmpty());
}

TEST(String, AssignALiteralStringInCtor)
{
	Baroque::String string("Hello World!");

	EXPECT_TRUE(string.IsShortString());
	EXPECT_EQ(string.Size(), Baroque::Unicode::ByteLength("Hello World!"));
	EXPECT_FALSE(string.IsEmpty());
	EXPECT_STREQ(string.Data(), "Hello World!");
}

TEST(String, LongLiteralStringShouldUseTheHeap)
{
	Baroque::String string("This is a long string that use the heap.");

	auto strLen = Baroque::Unicode::ByteLength("This is a long string that use the heap.");
	EXPECT_FALSE(string.IsShortString());
	EXPECT_EQ(string.Size(), strLen);
	EXPECT_STREQ(string.Data(), "This is a long string that use the heap.");
	EXPECT_EQ(string.Capacity(), strLen);
}

TEST(String, ConstCharPointerCtor)
{
	Baroque::String shortString(TestShortString);
	EXPECT_TRUE(shortString.IsShortString());
	EXPECT_EQ(shortString.Size(), Baroque::Unicode::ByteLength(TestShortString));
	EXPECT_STREQ(shortString.Data(), TestShortString);

	Baroque::String heapString(TestHeapString);
	EXPECT_FALSE(heapString.IsShortString());
	EXPECT_EQ(heapString.Size(), Baroque::Unicode::ByteLength(TestHeapString));
	EXPECT_STREQ(heapString.Data(), TestHeapString);
}

TEST(String, ConstCharPointerWithSizeCtor)
{
	const char* HelloWorld = "Hello World";
	const char* LongString = "This is a long string that will be truncated.";

	Baroque::String worldPart(HelloWorld + 6, 5);
	EXPECT_TRUE(worldPart.IsShortString());
	EXPECT_EQ(worldPart.Size(), 5);
	EXPECT_STREQ(worldPart.Data(), "World");

	Baroque::String truncatedString(LongString, 34);
	EXPECT_FALSE(truncatedString.IsShortString());
	EXPECT_EQ(truncatedString.Size(), 34);
	EXPECT_STREQ(truncatedString.Data(), "This is a long string that will be");
}

TEST(String, CopyCtorShortString)
{
	Baroque::String original(TestShortString);
	Baroque::String copy(original);

	EXPECT_TRUE(copy.IsShortString());
	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());
	EXPECT_NE(copy.Data(), original.Data());
	EXPECT_STREQ(copy.Data(), original.Data());
}

TEST(String, CopyCtorHeapString)
{
	Baroque::String original(TestHeapString);
	Baroque::String copy(original);

	EXPECT_FALSE(copy.IsShortString());
	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());
	EXPECT_NE(copy.Data(), original.Data());
	EXPECT_STREQ(copy.Data(), original.Data());
}

TEST(String, DeallocationIsCalledProperlyOnDtorForHeapString)
{
	VerifyDeallocImpl::Reset();

	void* stringData = nullptr;

	{
		VerifyDeallocString string(TestHeapString);
		stringData = string.Data();
	}

	EXPECT_NE(stringData, nullptr);
	EXPECT_EQ(VerifyDeallocImpl::LastDeallocation, stringData);
}

TEST(String, MoveCtorWithShortString)
{
	Baroque::String original(TestShortString);

	auto originalSize = original.Size();

	Baroque::String moved(std::move(original));

	EXPECT_EQ(original.Size(), 0);
	EXPECT_EQ(moved.Size(), originalSize);
	EXPECT_STREQ(moved.Data(), TestShortString);
}

TEST(String, MoveCtorWithHeapString)
{
	VerifyDeallocImpl::Reset();

	void* stringData = nullptr;
	void* originalData = nullptr;

	{
		VerifyDeallocString original(TestHeapString);

		auto originalSize = original.Size();
		auto originalCapacity = original.Capacity();
		originalData = original.Data();

		VerifyDeallocString moved(std::move(original));

		EXPECT_EQ(original.Size(), 0);
		EXPECT_NE(original.Capacity(), originalCapacity);

		EXPECT_EQ(moved.Size(), originalSize);
		EXPECT_EQ(moved.Capacity(), originalCapacity);
		EXPECT_STREQ(moved.Data(), TestHeapString);

		stringData = moved.Data();
	}

	EXPECT_EQ(stringData, originalData);
	EXPECT_EQ(VerifyDeallocImpl::DeallocationCount, 1);
	EXPECT_EQ(stringData, VerifyDeallocImpl::LastDeallocation);
}

TEST(String, CopyAssignmentShortToShort)
{
	Baroque::String original(TestShortString);

	Baroque::String copy(AnotherShortString);

	copy = original;

	EXPECT_TRUE(copy.IsShortString());
	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_STREQ(copy.Data(), original.Data());
}

TEST(String, CopyAssignmentShortToHeap)
{
	Baroque::String original(TestHeapString);

	Baroque::String copy(TestShortString);

	EXPECT_TRUE(copy.IsShortString());

	copy = original;

	EXPECT_FALSE(copy.IsShortString());
	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());
	EXPECT_STREQ(copy.Data(), original.Data());
}

TEST(String, CopyAssignmentHeapToHeap)
{
	VerifyDeallocString original(TestHeapString);

	VerifyDeallocString copy(AnotherHeapString);

	auto* stringData = copy.Data();

	copy = original;

	EXPECT_EQ(stringData, VerifyDeallocImpl::LastDeallocation);

	EXPECT_FALSE(copy.IsShortString());
	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());
	EXPECT_STREQ(copy.Data(), original.Data());
}

TEST(String, CopyAssignmentHeapToShort)
{
	VerifyDeallocString original(TestShortString);

	VerifyDeallocString copy(AnotherHeapString);

	auto* stringData = copy.Data();

	copy = original;

	EXPECT_EQ(stringData, VerifyDeallocImpl::LastDeallocation);

	EXPECT_TRUE(copy.IsShortString());
	EXPECT_EQ(copy.Size(), original.Size());
	EXPECT_EQ(copy.Capacity(), original.Capacity());
	EXPECT_STREQ(copy.Data(), original.Data());
}

TEST(String, MoveAssignmentShortToShort)
{
	Baroque::String original(TestShortString);

	Baroque::String moved(AnotherShortString);

	auto originalSize = original.Size();

	moved = std::move(original);
	
	EXPECT_EQ(original.Size(), 0);

	EXPECT_TRUE(moved.IsShortString());
	EXPECT_EQ(moved.Size(), originalSize);
	EXPECT_STREQ(moved.Data(), TestShortString);
}

TEST(String, MoveAssignmentShortToHeap)
{
	Baroque::String original(TestHeapString);

	Baroque::String moved(TestShortString);

	auto originalSize = original.Size();

	EXPECT_TRUE(moved.IsShortString());

	moved = std::move(original);

	EXPECT_EQ(original.Size(), 0);

	EXPECT_FALSE(moved.IsShortString());
	EXPECT_EQ(moved.Size(), originalSize);
	EXPECT_STREQ(moved.Data(), TestHeapString);
}

TEST(String, MoveAssignmentHeapToHeap)
{
	VerifyDeallocImpl::Reset();

	void* stringData = nullptr;
	void* originalData = nullptr;

	{
		VerifyDeallocString original(TestHeapString);

		originalData = original.Data();
		auto originalSize = original.Size();
		auto originalCapacity = original.Capacity();

		VerifyDeallocString moved(AnotherHeapString);

		auto oldMovedData = moved.Data();

		moved = std::move(original);

		EXPECT_EQ(oldMovedData, VerifyDeallocImpl::LastDeallocation);

		stringData = moved.Data();

		EXPECT_EQ(original.Size(), 0);

		EXPECT_FALSE(moved.IsShortString());
		EXPECT_EQ(moved.Size(), originalSize);
		EXPECT_EQ(moved.Capacity(), originalCapacity);
		EXPECT_STREQ(moved.Data(), TestHeapString);
	}

	EXPECT_EQ(stringData, originalData);
	EXPECT_EQ(stringData, VerifyDeallocImpl::LastDeallocation);
}

TEST(String, MoveAssignmentHeapToShort)
{
	VerifyDeallocString original(TestShortString);

	auto originalSize = original.Size();

	VerifyDeallocString moved(AnotherHeapString);

	EXPECT_FALSE(moved.IsShortString());

	auto* oldMoveData = moved.Data();

	moved = std::move(original);

	EXPECT_EQ(oldMoveData, VerifyDeallocImpl::LastDeallocation);

	EXPECT_EQ(original.Size(), 0);

	EXPECT_TRUE(moved.IsShortString());
	EXPECT_EQ(moved.Size(), originalSize);
	EXPECT_STREQ(moved.Data(), TestShortString);
}

TEST(String, AssignmentWithConstPtrShortString)
{
	Baroque::String string;

	string = TestShortString;

	EXPECT_TRUE(string.IsShortString());
	EXPECT_EQ(string.Size(), Baroque::Unicode::ByteLength(TestShortString));
	EXPECT_STREQ(string.Data(), TestShortString);
}

TEST(String, AssignmentWithConstPtrHeapString)
{
	Baroque::String string;

	string = TestHeapString;

	EXPECT_FALSE(string.IsShortString());
	EXPECT_EQ(string.Size(), Baroque::Unicode::ByteLength(TestHeapString));
	EXPECT_EQ(string.Capacity(), Baroque::Unicode::ByteLength(TestHeapString));
	EXPECT_STREQ(string.Data(), TestHeapString);
}

TEST(String, AssignmentWithConstPtrShouldDeleteOldString)
{
	VerifyDeallocImpl::Reset();

	VerifyDeallocString string(TestHeapString);

	auto oldData = string.Data();

	string = AnotherHeapString;

	EXPECT_EQ(VerifyDeallocImpl::LastDeallocation, oldData);

	EXPECT_FALSE(string.IsShortString());
	EXPECT_EQ(string.Size(), Baroque::Unicode::ByteLength(AnotherHeapString));
	EXPECT_EQ(string.Capacity(), Baroque::Unicode::ByteLength(AnotherHeapString));
	EXPECT_STREQ(string.Data(), AnotherHeapString);
}

TEST(String, AssignmentWithStringView)
{
	Baroque::String string;

	auto stringView = Baroque::StringView{ TestHeapString };
	string = stringView;

	EXPECT_EQ(string.Size(), stringView.Size());
	EXPECT_STREQ(string.Data(), stringView.Data());
}

TEST(String, AssignmentWithStringSpan)
{
	Baroque::String string;

	auto stringSpan = Baroque::StringSpan{ NonConstString };
	string = stringSpan;

	EXPECT_EQ(string.Size(), stringSpan.Size());
	EXPECT_STREQ(string.Data(), stringSpan.Data());
}

TEST(String, ClearShouldNotDeleteTheHeapString)
{
	VerifyDeallocImpl::Reset();

	VerifyDeallocString string(TestHeapString);

	string.Clear();

	EXPECT_EQ(VerifyDeallocImpl::LastDeallocation, nullptr);
}

TEST(String, ClearShouldZeroTheShortString)
{
	Baroque::String string(TestShortString);

	auto oldSize = string.Size();

	string.Clear();

	EXPECT_EQ(string.Size(), 0);

	auto* data = string.Data();
	for (std::size_t i = 0; i < oldSize; ++i)
	{
		EXPECT_EQ(data[i], '\0');
	}
}

TEST(String, ClearShouldZeroTheHeapString)
{
	Baroque::String string(TestHeapString);

	auto oldSize = string.Size();

	string.Clear();

	EXPECT_EQ(string.Size(), 0);

	for (std::size_t i = 0; i < oldSize; ++i)
	{
		EXPECT_EQ(string.Data()[i], '\0');
	}

	EXPECT_FALSE(string.IsShortString());
}

TEST(String, ReserveOnShortStringShouldChangeNothing)
{
	Baroque::String shortString;

	shortString.Reserve(12);

	EXPECT_NE(shortString.Capacity(), 12);
}

TEST(String, ReverseOnHeapStringShouldRealloc)
{
	VerifyDeallocImpl::Reset();

	VerifyDeallocString heapString(TestHeapString);

	auto oldCapacity = heapString.Capacity();
	auto* oldData = heapString.Data();

	heapString.Reserve(oldCapacity * 3);

	EXPECT_EQ(VerifyDeallocImpl::LastDeallocation, oldData);

	EXPECT_NE(oldData, heapString.Data());
	EXPECT_EQ(heapString.Capacity(), oldCapacity * 3);
}

TEST(String, AppendASingleChar)
{
	Baroque::String string;

	EXPECT_TRUE(string.IsShortString());
	EXPECT_EQ(string.Size(), 0);

	string.Append('A');

	EXPECT_EQ(string.Size(), 1);
	EXPECT_STREQ(string.Data(), "A");
}

TEST(String, AppendAnotherString)
{
	Baroque::String result{ "Hello" };

	result.Append(Baroque::String{ " World" });

	EXPECT_EQ(result.Size(), 11);
	EXPECT_STREQ(result.Data(), "Hello World");
}

TEST(String, AppendConstPtr)
{
	Baroque::String result{ "Hello" };

	result.Append(" World");

	EXPECT_EQ(result.Size(), 11);
	EXPECT_STREQ(result.Data(), "Hello World");
}

TEST(String, AppendStringView)
{
	Baroque::String result{ "Hello" };

	result.Append(Baroque::StringView{ " World" });

	EXPECT_EQ(result.Size(), 11);
	EXPECT_STREQ(result.Data(), "Hello World");
}

TEST(String, AppendShouldMoveToHeapWhenExceedingShortSize)
{
	Baroque::String string("ABCDEFGHIJKLMNOPQRSTUV");

	EXPECT_TRUE(string.IsShortString());

	auto* oldData = string.Data();

	string.Append("WXYZ0123456789");

	EXPECT_FALSE(string.IsShortString());
	EXPECT_NE(oldData, string.Data());
	EXPECT_STREQ(string.Data(), "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
}

TEST(String, AppendByCodepoint)
{
	using namespace Baroque::Unicode;

	{
		const char Expected[] = "A";

		Baroque::String string;

		string.Append(U'A'_cp);

		EXPECT_EQ(string.Size(), 1);
		EXPECT_EQ(string.CodepointLength(), 1);

		EXPECT_STREQ(string.Data(), Expected);

		EXPECT_EQ(string.CodepointAt(0), Baroque::Unicode::Codepoint{ 0x0041 });
	}

	{
		const char Expected[] = "\xC3\xA9";

		Baroque::String string;

		string.Append(U'é'_cp);

		EXPECT_EQ(string.Size(), 2);
		EXPECT_EQ(string.CodepointLength(), 1);

		EXPECT_STREQ(string.Data(), Expected);

		EXPECT_EQ(string.CodepointAt(0), Baroque::Unicode::Codepoint{ 0x00E9 });
	}

	{
		const char Expected[] = "\xE9\x9B\xBB";

		Baroque::String string;

		string.Append(U'電'_cp);

		EXPECT_EQ(string.Size(), 3);
		EXPECT_EQ(string.CodepointLength(), 1);

		EXPECT_STREQ(string.Data(), Expected);
		
		EXPECT_EQ(string.CodepointAt(0), Baroque::Unicode::Codepoint{ 0x96FB });
	}

	{
		const char Expected[] = "\xF0\x9F\x98\x80";

		Baroque::String string;

		string.Append(U'😀'_cp);

		EXPECT_EQ(string.Size(), 4);
		EXPECT_EQ(string.CodepointLength(), 1);

		EXPECT_STREQ(string.Data(), Expected);

		EXPECT_EQ(string.CodepointAt(0), Baroque::Unicode::Codepoint{ 0x1F600 });
	}
}

TEST(String, ForRangeLoop)
{
	Baroque::String string(TestHeapString);

	std::size_t index = 0;
	for (auto character : string)
	{
		EXPECT_EQ(character, TestHeapString[index]);
		++index;
	}
}

TEST(String, ArraySubscriptOperator)
{
	Baroque::String string(TestShortString);

	EXPECT_EQ(string[0], TestShortString[0]);

	string[1] = 'S';
	string[2] = 'O';

	EXPECT_EQ(string[1], 'S');
	EXPECT_EQ(string[2], 'O');
}

TEST(String, ByCodepoint)
{
	Baroque::Unicode::Codepoint Expected[] = {
		0x0041,
		0x00E9,
		0x96FB,
		0x1F600
	};

	Baroque::String unicodeString{ u8"Aé電😀" };

	std::size_t index = 0;
	for (auto codepoint : unicodeString.ByCodepoint())
	{
		EXPECT_EQ(codepoint, Expected[index]);
		++index;
	}
}

TEST(String, CodepointLength)
{
	Baroque::String unicodeString{ u8"Aé電😀" };

	EXPECT_EQ(unicodeString.CodepointLength(), 4);
}

TEST(String, CodepointAt)
{
	Baroque::String unicodeString{ u8"Aé電😀" };

	EXPECT_EQ(unicodeString.CodepointAt(0), Baroque::Unicode::Codepoint{ 0x0041 });
	EXPECT_EQ(unicodeString.CodepointAt(1), Baroque::Unicode::Codepoint{ 0x00E9 });
	EXPECT_EQ(unicodeString.CodepointAt(2), Baroque::Unicode::Codepoint{ 0x96FB });
	EXPECT_EQ(unicodeString.CodepointAt(3), Baroque::Unicode::Codepoint{ 0x1F600 });
}

TEST(String, InsertChar)
{
	Baroque::String string{ "12345678" };

	string.Insert(1, 'A');

	EXPECT_EQ(string.Size(), 9);
	EXPECT_STREQ(string.Data(), "1A2345678");
}

TEST(String, InsertCharShouldMoveFromShortToHeap)
{
	Baroque::String string{ "ABCDEFGHIJKLMNOPQRSTUVW" };

	EXPECT_TRUE(string.IsShortString());

	auto* oldData = string.Data();

	string.Insert(0, '0');

	EXPECT_EQ(string.Size(), 24);
	EXPECT_FALSE(string.IsShortString());
	EXPECT_NE(string.Data(), oldData);
	EXPECT_STREQ(string.Data(), "0ABCDEFGHIJKLMNOPQRSTUVW");
}

TEST(String, InsertCodepoint)
{
	using namespace Baroque::Unicode;

	Baroque::String string{ "12345678" };

	string.Insert(4, U'火'_cp);

	const char* Expected = u8"1234火5678";

	EXPECT_EQ(string.Size(), 11);
	EXPECT_EQ(string.CodepointLength(), 9);
	EXPECT_STREQ(string.Data(), Expected);
}

TEST(String, InsertCodepointShouldMoveFromShortToHeap)
{
	using namespace Baroque::Unicode;

	Baroque::String string{ "ABCDEFGHIJKLMNOPQRSTUVW" };

	EXPECT_TRUE(string.IsShortString());

	auto* oldData = string.Data();

	string.Insert(string.Size(), U'é'_cp);

	EXPECT_EQ(string.Size(), 25);
	EXPECT_FALSE(string.IsShortString());
	EXPECT_NE(string.Data(), oldData);
	EXPECT_STREQ(string.Data(), u8"ABCDEFGHIJKLMNOPQRSTUVWé");
}

TEST(String, InsertConstPtr)
{
	Baroque::String string{ "Hello" };

	string.Insert(0, "World ");

	EXPECT_EQ(string.Size(), 11);
	EXPECT_STREQ(string.Data(), "World Hello");
}

TEST(String, InsertConstPtrUtf8)
{
	Baroque::String string{ "Hello World!" };

	string.Insert(6, u8"こんにちは！ ");

	const char* Expected = u8"Hello こんにちは！ World!";

	EXPECT_EQ(string.Size(), 31);
	EXPECT_EQ(string.CodepointLength(), 19);
	EXPECT_STREQ(string.Data(), Expected);
}

TEST(String, InsertAnotherString)
{
	Baroque::String string{ "Hello" };
	Baroque::String world{ "World! " };

	string.Insert(string.Size(), world);

	EXPECT_EQ(string.Size(), 12);
	EXPECT_STREQ(string.Data(), "HelloWorld! ");
}

TEST(String, InsertStringView)
{
	Baroque::String string{ "Hello" };

	string.Insert(string.Size(), Baroque::StringView{ " World!" });

	EXPECT_EQ(string.Size(), 12);
	EXPECT_STREQ(string.Data(), "Hello World!");
}

TEST(String, InsertWithStringViewShouldMoveFromShortToHeap)
{
	Baroque::String string{ "ABCDEFGHIJKLMNOPQRSTUVW" };

	EXPECT_TRUE(string.IsShortString());

	auto* oldData = string.Data();

	string.Insert(string.Size(), Baroque::StringView{ "YXZ0123456789" });

	EXPECT_FALSE(string.IsShortString());
	EXPECT_EQ(string.Size(), 36);
	EXPECT_NE(string.Data(), oldData);
	EXPECT_STREQ(string.Data(), "ABCDEFGHIJKLMNOPQRSTUVWYXZ0123456789");
}

TEST(String, RemoveChar)
{
	Baroque::String string{ "Hello World!" };

	auto removedCount = string.Remove('l');

	EXPECT_EQ(removedCount, 3);
	EXPECT_EQ(string.Size(), 9);
	EXPECT_STREQ(string.Data(), "Heo Word!");
}

TEST(String, RemoveCodepoint)
{
	using namespace Baroque::Unicode;

	Baroque::String string{ u8"こんにちは！こんばんは！おはよう！" };

	auto removedCount = string.Remove(U'は'_cp);

	const char* Expected = u8"こんにち！こんばん！およう！";

	EXPECT_EQ(removedCount, 3);
	EXPECT_EQ(string.Size(), 42);
	EXPECT_EQ(string.CodepointLength(), 14);
	EXPECT_STREQ(string.Data(), Expected);
}

TEST(String, RemoveAt)
{
	Baroque::String string{ "Hello World!" };

	string.RemoveAt(5);

	EXPECT_EQ(string.Size(), 11);
	EXPECT_STREQ(string.Data(), "HelloWorld!");
}

TEST(String, RemoveAtCodepoint)
{
	Baroque::String string{ u8"こんにちは！こんばんは！おはよう！" };

	string.RemoveAtCodepoint(5);

	const char* Expected = u8"こんにちはこんばんは！おはよう！";

	EXPECT_EQ(string.Size(), 48);
	EXPECT_EQ(string.CodepointLength(), 16);
	EXPECT_STREQ(string.Data(), Expected);
}

TEST(String, Front)
{
	Baroque::String string{ "HelloWorld" };

	string.Front() = 'A';

	EXPECT_STREQ(string.Data(), "AelloWorld");
}

TEST(String, Last)
{
	Baroque::String string{ "HelloWorld" };

	string.Last() = '!';

	EXPECT_STREQ(string.Data(), "HelloWorl!");
}

TEST(String, ContainsChar)
{
	Baroque::String string;

	EXPECT_FALSE(string.Contains('W'));

	string = "Hello!";

	EXPECT_FALSE(string.Contains('W'));

	string = "Hello World!";

	EXPECT_TRUE(string.Contains('W'));
}

TEST(String, ContainsCodepoint)
{
	using namespace Baroque::Unicode;

	Baroque::String string{ u8"こんにち" };

	auto codepoint = U'は'_cp;

	EXPECT_FALSE(string.Contains(codepoint));

	string = u8"こんにちは！こんばんは！おはよう！";

	EXPECT_TRUE(string.Contains(codepoint));
}

TEST(String, ContainsString)
{
	Baroque::String string;

	EXPECT_FALSE(string.Contains("World"));

	string = "Hello!";

	EXPECT_FALSE(string.Contains("World"));

	string = "Hello World!";

	EXPECT_TRUE(string.Contains("World"));
}

TEST(String, ContainsStringUnicode)
{
	const char* Substring = u8"こんばんは";

	Baroque::String string;

	EXPECT_FALSE(string.Contains(Substring));

	string = u8"こんにちは！";

	EXPECT_FALSE(string.Contains(Substring));

	string = u8"こんにちは！こんばんは！おはよう！";

	EXPECT_TRUE(string.Contains(Substring));
}

TEST(String, IndexOfChar)
{
	Baroque::String string;

	EXPECT_EQ(string.IndexOf('W'), string.Size());

	string = "Hello!";

	EXPECT_EQ(string.IndexOf('W'), string.Size());

	string = "Hello World!";

	EXPECT_EQ(string.IndexOf('W'), 6);
}

TEST(String, IndexOfCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr auto Codepoint = U'は'_cp;

	Baroque::String string{ u8"こんにち" };

	EXPECT_EQ(string.IndexOf(Codepoint), string.Size());

	string = u8"こんにちは！こんばんは！おはよう！";

	EXPECT_EQ(string.IndexOf(Codepoint), 12);
}

TEST(String, IndexOfString)
{
	Baroque::String string;

	EXPECT_EQ(string.IndexOf("World"), string.Size());

	string = "Hello!";

	EXPECT_EQ(string.IndexOf("World"), string.Size());

	string = "Hello World!";

	EXPECT_EQ(string.IndexOf("World"), 6);
}

TEST(String, IndexOfStringUnicode)
{
	const char* Substring = u8"こんばんは";

	Baroque::String string;

	EXPECT_EQ(string.IndexOf(Substring), string.Size());

	string = u8"こんにちは！";

	EXPECT_EQ(string.IndexOf(Substring), string.Size());

	string = u8"こんにちは！こんばんは！おはよう！";

	EXPECT_EQ(string.IndexOf(Substring), 18);
}

TEST(String, LastIndexOfChar)
{
	Baroque::String string;

	EXPECT_EQ(string.LastIndexOf('.'), string.Size());

	string = "NoExtension";

	EXPECT_EQ(string.LastIndexOf('.'), string.Size());

	string = "Sample.tar.gz";

	EXPECT_EQ(string.LastIndexOf('.'), 10);
}

TEST(String, LastIndexOfCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr const auto Codepoint = U'¥'_cp;

	Baroque::String string;

	EXPECT_EQ(string.LastIndexOf(Codepoint), string.Size());

	string = u8"SomeFilenameデータファイル";

	EXPECT_EQ(string.LastIndexOf(Codepoint), string.Size());

	string = u8"Parent¥Folder¥SomeFilename.dat";

	EXPECT_EQ(string.LastIndexOf(Codepoint), 14);
}

TEST(String, LastIndexOfString)
{
	Baroque::String string;

	EXPECT_EQ(string.LastIndexOf("Windows"), string.Size());

	string = "DoesNotContains";

	EXPECT_EQ(string.LastIndexOf("Windows"), string.Size());

	string = "Windows 95, Windows 10";

	EXPECT_EQ(string.LastIndexOf("Windows"), 12);
}

TEST(String, LastIndexOfStringUnicode)
{
	const char* Substring = u8"大きい";

	Baroque::String string;

	EXPECT_EQ(string.LastIndexOf(Substring), string.Size());

	string = u8"小さい";

	EXPECT_EQ(string.LastIndexOf(Substring), string.Size());

	string = u8"大きい、小さい、大きい";

	EXPECT_EQ(string.LastIndexOf(Substring), 24);
}

TEST(String, FindChar)
{
	Baroque::String string;

	EXPECT_EQ(string.Find('W'), nullptr);

	string = "Hello!";

	EXPECT_EQ(string.Find('W'), nullptr);

	string = "Hello World!";

	auto foundIt = string.Find('W');
	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - string.Data(), 6);
	EXPECT_EQ(*foundIt, 'W');
}

TEST(String, FindCharConst)
{
	const Baroque::String empty;

	EXPECT_EQ(empty.Find('W'), nullptr);

	const Baroque::String invalid = "Hello!";

	EXPECT_EQ(invalid.Find('W'), nullptr);

	const Baroque::String valid = "Hello World!";

	auto foundIt = valid.Find('W');
	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - valid.Data(), 6);
	EXPECT_EQ(*foundIt, 'W');
}

TEST(String, FindCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr auto Codepoint = U'は'_cp;

	Baroque::String empty;
	EXPECT_EQ(empty.Find(Codepoint), nullptr);

	Baroque::String string{ u8"こんにち" };

	EXPECT_EQ(string.Find(Codepoint), nullptr);

	string = u8"こんにちは！こんばんは！おはよう！";

	auto foundIt = string.Find(Codepoint);

	EXPECT_EQ(*CodepointIterator(foundIt), Codepoint);
	EXPECT_EQ(foundIt - string.Data(), 12);
}

TEST(String, FindCodepointConst)
{
	using namespace Baroque::Unicode;

	constexpr auto Codepoint = U'は'_cp;

	const Baroque::String empty;
	EXPECT_EQ(empty.Find(Codepoint), nullptr);

	const Baroque::String invalid{ u8"こんにち" };
	EXPECT_EQ(invalid.Find(Codepoint), nullptr);

	const Baroque::String valid = u8"こんにちは！こんばんは！おはよう！";

	auto foundIt = valid.Find(Codepoint);

	EXPECT_EQ(*CodepointIterator(foundIt), Codepoint);
	EXPECT_EQ(foundIt - valid.Data(), 12);
}

TEST(String, FindString)
{
	Baroque::String string;

	EXPECT_EQ(string.Find("World"), nullptr);

	string = "Hello!";

	EXPECT_EQ(string.Find("World"), nullptr);

	string = "Hello World!";

	auto foundIt = string.Find("World");
	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - string.Data(), 6);
	EXPECT_EQ(*foundIt, 'W');
}

TEST(String, FindStringConst)
{
	const Baroque::String empty;
	EXPECT_EQ(empty.Find("World"), nullptr);

	const Baroque::String invalid = "Hello!";
	EXPECT_EQ(invalid.Find("World"), nullptr);

	const Baroque::String valid = "Hello World!";

	auto foundIt = valid.Find("World");
	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - valid.Data(), 6);
	EXPECT_EQ(*foundIt, 'W');
}

TEST(String, FindStringUnicode)
{
	using namespace Baroque::Unicode;

	const char* Substring = u8"こんばんは";
	constexpr auto ExpectedCodepoint = U'こ'_cp;

	Baroque::String string;

	EXPECT_EQ(string.Find(Substring), nullptr);

	string = u8"こんにちは！";

	EXPECT_EQ(string.Find(Substring), nullptr);

	string = u8"こんにちは！こんばんは！おはよう！";

	auto foundIt = string.Find(Substring);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - string.Data(), 18);
	EXPECT_EQ(*CodepointIterator(foundIt), ExpectedCodepoint);
}

TEST(String, FindStringUnicodeConst)
{
	using namespace Baroque::Unicode;

	const char* Substring = u8"こんばんは";
	constexpr auto ExpectedCodepoint = U'こ'_cp;

	const Baroque::String empty;
	EXPECT_EQ(empty.Find(Substring), nullptr);

	const Baroque::String invalid = u8"こんにちは！";
	EXPECT_EQ(invalid.Find(Substring), nullptr);

	const Baroque::String valid = u8"こんにちは！こんばんは！おはよう！";

	auto foundIt = valid.Find(Substring);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - valid.Data(), 18);
	EXPECT_EQ(*CodepointIterator(foundIt), ExpectedCodepoint);
}

TEST(String, ReverseFindChar)
{
	Baroque::String string;

	EXPECT_EQ(string.ReverseFind('.'), nullptr);

	string = "NoExtension";

	EXPECT_EQ(string.ReverseFind('.'), nullptr);

	string = "Sample.tar.gz";

	auto foundIt = string.ReverseFind('.');

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - string.Data(), 10);
	EXPECT_EQ(*foundIt, '.');
}

TEST(String, ReverseFindCharConst)
{
	const Baroque::String empty;

	EXPECT_EQ(empty.ReverseFind('.'), nullptr);

	const Baroque::String noExtension = "NoExtension";

	EXPECT_EQ(noExtension.ReverseFind('.'), nullptr);

	const Baroque::String valid = "Sample.tar.gz";

	auto foundIt = valid.ReverseFind('.');

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(foundIt - valid.Data(), 10);
	EXPECT_EQ(*foundIt, '.');
}

TEST(String, ReverseFindCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr const auto Codepoint = U'¥'_cp;

	Baroque::String string;

	EXPECT_EQ(string.ReverseFind(Codepoint), nullptr);

	string = u8"SomeFilenameデータファイル";

	EXPECT_EQ(string.ReverseFind(Codepoint), nullptr);

	string = u8"Parent¥Folder¥SomeFilename.dat";

	auto foundIt = string.ReverseFind(Codepoint);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*CodepointIterator(foundIt), Codepoint);
	EXPECT_EQ(foundIt - string.Data(), 14);
}

TEST(String, ReverseFindCodepointConst)
{
	using namespace Baroque::Unicode;

	constexpr const auto Codepoint = U'¥'_cp;

	const Baroque::String empty;

	EXPECT_EQ(empty.ReverseFind(Codepoint), nullptr);

	const Baroque::String noExtension = u8"SomeFilenameデータファイル";

	EXPECT_EQ(noExtension.ReverseFind(Codepoint), nullptr);

	const Baroque::String valid = u8"Parent¥Folder¥SomeFilename.dat";

	auto foundIt = valid.ReverseFind(Codepoint);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*CodepointIterator(foundIt), Codepoint);
	EXPECT_EQ(foundIt - valid.Data(), 14);
}

TEST(String, ReverseFindString)
{
	Baroque::String string;

	EXPECT_EQ(string.ReverseFind("Windows"), nullptr);

	string = "DoesNotContains";

	EXPECT_EQ(string.ReverseFind("Windows"), nullptr);

	string = "Windows 95, Windows 10";

	auto foundIt = string.ReverseFind("Windows");

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*foundIt, 'W');
	EXPECT_EQ(foundIt - string.Data(), 12);
}

TEST(String, ReverseFindStringConst)
{
	const Baroque::String empty;

	EXPECT_EQ(empty.ReverseFind("Windows"), nullptr);

	const Baroque::String doesNotContains = "DoesNotContains";

	EXPECT_EQ(doesNotContains.ReverseFind("Windows"), nullptr);

	const Baroque::String valid = "Windows 95, Windows 10";

	auto foundIt = valid.ReverseFind("Windows");

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*foundIt, 'W');
	EXPECT_EQ(foundIt - valid.Data(), 12);
}

TEST(String, ReverseFindStringUnicode)
{
	using namespace Baroque::Unicode;

	const char* Substring = u8"大きい";
	constexpr auto FirstCodepoint = U'大'_cp;

	Baroque::String string;

	EXPECT_EQ(string.ReverseFind(Substring), nullptr);

	string = u8"小さい";

	EXPECT_EQ(string.ReverseFind(Substring), nullptr);

	string = u8"大きい、小さい、大きい";

	auto foundIt = string.ReverseFind(Substring);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*(CodepointIterator(foundIt)), FirstCodepoint);
	EXPECT_EQ(foundIt - string.Data(), 24);
}

TEST(String, ReverseFindStringUnicodeConst)
{
	using namespace Baroque::Unicode;

	const char* Substring = u8"大きい";
	constexpr auto FirstCodepoint = U'大'_cp;

	const Baroque::String empty;

	EXPECT_EQ(empty.ReverseFind(Substring), nullptr);

	const Baroque::String invalid = u8"小さい";

	EXPECT_EQ(invalid.ReverseFind(Substring), nullptr);

	const Baroque::String valid = u8"大きい、小さい、大きい";

	auto foundIt = valid.ReverseFind(Substring);

	EXPECT_NE(foundIt, nullptr);
	EXPECT_EQ(*(CodepointIterator(foundIt)), FirstCodepoint);
	EXPECT_EQ(foundIt - valid.Data(), 24);
}

TEST(String, Erase)
{
	Baroque::String string{ "Hello World!" };

	auto it = string.begin();

	std::size_t eraseCount = 0;

	while (it != string.end())
	{
		if (*it == 'l')
		{
			it = string.Erase(it);
			++eraseCount;
		}
		else
		{
			++it;
		}
	}

	EXPECT_EQ(eraseCount, 3);
	EXPECT_EQ(string.Size(), 9);
	EXPECT_STREQ(string.Data(), "Heo Word!");
}

TEST(String, EraseCodepoint)
{
	using namespace Baroque::Unicode;

	const char* Expected = u8"こんにち！こんばん！およう！";
	constexpr auto CodepointToErase = U'は'_cp;

	Baroque::String string{ u8"こんにちは！こんばんは！おはよう！" };

	auto codepointIt = string.CodepointBegin();
	
	std::size_t eraseCount = 0;

	while (codepointIt != string.CodepointEnd())
	{
		if (*codepointIt == CodepointToErase)
		{
			codepointIt = string.Erase(codepointIt);
			++eraseCount;
		}
		else
		{
			++codepointIt;
		}
	}

	EXPECT_EQ(eraseCount, 3);
	EXPECT_EQ(string.Size(), 42);
	EXPECT_EQ(string.CodepointLength(), 14);
	EXPECT_STREQ(string.Data(), Expected);
}

TEST(String, StartsWithChar)
{
	Baroque::String string;

	EXPECT_FALSE(string.StartsWith('H'));

	string = "Hello World!";

	EXPECT_TRUE(string.StartsWith('H'));
	EXPECT_FALSE(string.StartsWith('W'));
}

TEST(String, StartsWithCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr auto CorrectCodepoint = U'こ'_cp;
	constexpr auto BadCodepoint = U'は'_cp;

	Baroque::String string;

	EXPECT_FALSE(string.StartsWith(CorrectCodepoint));

	string = u8"こんにちは！";

	EXPECT_TRUE(string.StartsWith(CorrectCodepoint));
	EXPECT_FALSE(string.StartsWith(BadCodepoint));
}

TEST(String, StartsWithString)
{
	Baroque::String string;

	EXPECT_FALSE(string.StartsWith("Hello"));

	string = "Hello World!";

	EXPECT_TRUE(string.StartsWith("Hello"));
	EXPECT_FALSE(string.StartsWith("World"));
}

TEST(String, StartsWithStringUnicode)
{
	const char* ExpectedSubstring = u8"こんにちは";
	const char* NotExpectedSubstring = u8"こんばんは";

	Baroque::String string;

	EXPECT_FALSE(string.StartsWith(ExpectedSubstring));

	string = u8"こんにちは！　よろしくお願いします！";

	EXPECT_TRUE(string.StartsWith(ExpectedSubstring));
	EXPECT_FALSE(string.StartsWith(NotExpectedSubstring));
}

TEST(String, EndsWithChar)
{
	Baroque::String string;

	EXPECT_FALSE(string.EndsWith('!'));

	string = "Hi";

	EXPECT_FALSE(string.EndsWith('!'));

	string = "Hello World !";

	EXPECT_TRUE(string.EndsWith("!"));
}

TEST(String, EndsWithCodepoint)
{
	using namespace Baroque::Unicode;

	constexpr auto ExpectedCodepoint = U'。'_cp;

	Baroque::String string;

	EXPECT_FALSE(string.EndsWith(ExpectedCodepoint));

	string = u8"バロックだ！";

	EXPECT_FALSE(string.EndsWith(ExpectedCodepoint));

	string = u8"バロックです。";

	EXPECT_TRUE(string.EndsWith(ExpectedCodepoint));
}

TEST(String, EndsWithString)
{
	const char* ExpectedSubstring = ".txt";

	Baroque::String string;

	EXPECT_FALSE(string.EndsWith(ExpectedSubstring));

	string = "NotAValidFilename";

	EXPECT_FALSE(string.EndsWith(ExpectedSubstring));

	string = "Test.txt";

	EXPECT_TRUE(string.EndsWith(ExpectedSubstring));
}

TEST(String, EndsWithStringUnicode)
{
	const char* ExpectedSubstring = u8"です。";

	Baroque::String string;

	EXPECT_FALSE(string.EndsWith(ExpectedSubstring));

	string = u8"バロックだ！";

	EXPECT_FALSE(string.EndsWith(ExpectedSubstring));

	string = u8"バロックです。";

	EXPECT_TRUE(string.EndsWith(ExpectedSubstring));
}

TEST(String, ResizeUpNoReallocation)
{
	Baroque::String string{ "Ab" };

	auto* previousData = string.Data();

	string.Resize(4);

	EXPECT_EQ(string.Size(), 4);
	EXPECT_STREQ(string.Data(), "Ab");
	EXPECT_EQ(string.Data(), previousData);
}

TEST(String, ResizeUpReallocation)
{
	Baroque::String string{ "Small String" };

	EXPECT_TRUE(string.IsShortString());

	auto* previousData = string.Data();

	string.Resize(64);

	EXPECT_EQ(string.Size(), 64);
	EXPECT_NE(string.Data(), previousData);
	EXPECT_STREQ(string.Data(), "Small String");
}

TEST(String, ResizeDown)
{
	Baroque::String string{ "Small String" };

	auto oldSize = string.Size();

	string.Resize(5);

	auto* data = string.Data();

	for (std::size_t i = 5; i < oldSize; ++i)
	{
		EXPECT_EQ(data[i], '\0');
	}

	EXPECT_STREQ(string.Data(), "Small");
}

TEST(String, SubstringWithStart)
{
	Baroque::String full{ "C:\\First\\Second\\Test.txt" };

	auto fileName = full.Substring(full.LastIndexOf('\\') + 1);

	EXPECT_EQ(fileName.Size(), 8);
	EXPECT_EQ(fileName, "Test.txt");
}

TEST(String, SubstringWithStartAndCount)
{
	Baroque::String full{ "C:\\First\\Second\\Test.txt" };

	auto fileName = full.Substring(full.LastIndexOf('\\') + 1, 4);

	EXPECT_EQ(fileName.Size(), 4);
	EXPECT_EQ(fileName, "Test");
}

TEST(String, Slice)
{
	Baroque::String full{ "0123456789" };

	auto slice = full.Slice(1, 3);

	EXPECT_EQ(slice.Size(), 2);
	EXPECT_EQ(slice, "12");
}

TEST(String, ToStringView)
{
	Baroque::String string(TestShortString);

	auto view = string.ToStringView();

	EXPECT_EQ(view.Size(), string.Size());
	EXPECT_EQ(view.Data(), string.Data());
	EXPECT_EQ(view.end(), string.end());
	EXPECT_STREQ(view.Data(), string.Data());
}

TEST(String, ToStringSpan)
{
	Baroque::String string(TestShortString);

	auto span = string.ToStringSpan();

	EXPECT_EQ(span.Size(), string.Size());
	EXPECT_EQ(span.Data(), string.Data());
	EXPECT_EQ(span.end(), string.end());
	EXPECT_STREQ(span.Data(), string.Data());
}

TEST(String, TrimBeginNonStatic)
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
	full.Insert(0, Baroque::Unicode::Codepoint{0x2028});
	full.Insert(0, Baroque::Unicode::Codepoint{0x2029});
	full.Insert(0, Baroque::Unicode::Codepoint{0x202F});
	full.Insert(0, Baroque::Unicode::Codepoint{0x205F});
	full.Insert(0, Baroque::Unicode::Codepoint{0x3000});

	full.TrimBegin();

	EXPECT_EQ(full.Size(), 9);
	EXPECT_STREQ(full.Data(), "Trim Test");
}

TEST(String, TrimBeginStatic)
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

	auto fullSize = full.Size();

	auto trimmed = Baroque::String::TrimBegin(full);
	auto trimmed2 = full.TrimBegin(full);

	EXPECT_EQ(full.Size(), fullSize);
	EXPECT_EQ(trimmed.Size(), 9);
	EXPECT_STREQ(trimmed.Data(), "Trim Test");
	EXPECT_EQ(trimmed2.Size(), 9);
	EXPECT_STREQ(trimmed2.Data(), "Trim Test");
}

TEST(String, TrimEndNonStatic)
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

	full.TrimEnd();

	EXPECT_EQ(full.Size(), 9);
	EXPECT_STREQ(full.Data(), "Trim Test");
}

TEST(String, TrimEndStatic)
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

	auto trimmed = Baroque::String::TrimEnd(full);
	auto trimmed2 = full.TrimEnd(full);

	EXPECT_EQ(trimmed.Size(), 9);
	EXPECT_STREQ(trimmed.Data(), "Trim Test");

	EXPECT_EQ(trimmed2.Size(), 9);
	EXPECT_STREQ(trimmed2.Data(), "Trim Test");
}

TEST(String, TrimNonStatic)
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

	full.Trim();

	EXPECT_EQ(full.Size(), 9);
	EXPECT_STREQ(full.Data(), "Trim Test");
}

TEST(String, ReplaceCharacter)
{
	Baroque::String string{ "Hello World" };

	string.Replace('l', 'c');

	EXPECT_EQ(string.Size(), 11);
	EXPECT_STREQ(string.Data(), "Hecco Worcd");
}

TEST(String, ReplaceCodepoint)
{
	using namespace Baroque::Unicode;

	const char* TwoToThreeTwoExpected = u8"Test デデータデ Testing";
	const char* ThreeToTwoExpected = u8"Test ééータé Testing";
	const char* OneToFourExpected = u8"Test 😀 Testing";
	const char* SameLengthExpected = u8"Test ààータà Testing";

	Baroque::String twoToThree{ ThreeToTwoExpected };
	twoToThree.Replace(U'é'_cp, U'デ'_cp);
	EXPECT_EQ(twoToThree.Size(), 28);
	EXPECT_STREQ(twoToThree.Data(), TwoToThreeTwoExpected);

	Baroque::String threeToTwo{ TwoToThreeTwoExpected };
	threeToTwo.Replace(U'デ'_cp, U'é'_cp);
	EXPECT_EQ(threeToTwo.Size(), 25);
	EXPECT_STREQ(threeToTwo.Data(),  ThreeToTwoExpected);

	Baroque::String sameLength{ ThreeToTwoExpected };
	sameLength.Replace(U'é'_cp, U'à'_cp);
	EXPECT_EQ(sameLength.Size(), 25);
	EXPECT_STREQ(sameLength.Data(), SameLengthExpected);

	Baroque::String oneToFour{ "Test A Testing" };
	oneToFour.Replace(U'A'_cp, U'😀'_cp);
	EXPECT_EQ(oneToFour.Size(), 17);
	EXPECT_STREQ(oneToFour.Data(), OneToFourExpected);
}

TEST(String, ReplaceSubstringWithNull)
{
	Baroque::String string{ "World Hello World" };

	string.Replace("World", Baroque::StringView{});

	EXPECT_EQ(string.Size(), 7);
	EXPECT_STREQ(string.Data(), " Hello ");
}

TEST(String, ReplaceSubstringWithEmpty)
{
	Baroque::String string{ "World Hello World" };

	string.Replace("World", "");

	EXPECT_EQ(string.Size(), 7);
	EXPECT_STREQ(string.Data(), " Hello ");
}

TEST(String, ReplaceSubstringWithSameLengthString)
{
	Baroque::String string{ "World Hello World" };

	string.Replace("World", "Monde");

	EXPECT_EQ(string.Size(), 17);
	EXPECT_STREQ(string.Data(), "Monde Hello Monde");
}

TEST(String, ReplaceSubstringWithShorterString)
{
	Baroque::String string{ "World Hello World" };

	string.Replace("World", "Te");

	EXPECT_EQ(string.Size(), 11);
	EXPECT_STREQ(string.Data(), "Te Hello Te");
}

TEST(String, ReplaceSubstringWithLongerString)
{
	Baroque::String string{ "World Hello World" };

	string.Replace("World", "TerraMondeAlpha");

	EXPECT_EQ(string.Size(), 37);
	EXPECT_STREQ(string.Data(), "TerraMondeAlpha Hello TerraMondeAlpha");
}

TEST(String, StaticReplaceCharacter)
{
	auto replaced = Baroque::String::Replace("Hello World", 'l', 'c');

	EXPECT_EQ(replaced.Size(), 11);
	EXPECT_STREQ(replaced.Data(), "Hecco Worcd");
}

TEST(String, StaticReplaceCodepoint)
{
	using namespace Baroque::Unicode;

	const char* TwoToThreeTwoExpected = u8"Test デデータデ Testing";
	const char* ThreeToTwoExpected = u8"Test ééータé Testing";
	const char* OneToFourExpected = u8"Test 😀 Testing";
	const char* SameLengthExpected = u8"Test ààータà Testing";

	Baroque::String replaced;

	replaced = Baroque::String::Replace(ThreeToTwoExpected, U'é'_cp, U'デ'_cp);
	EXPECT_EQ(replaced.Size(), 28);
	EXPECT_STREQ(replaced.Data(), TwoToThreeTwoExpected);

	replaced = Baroque::String::Replace(TwoToThreeTwoExpected, U'デ'_cp, U'é'_cp);
	EXPECT_EQ(replaced.Size(), 25);
	EXPECT_STREQ(replaced.Data(), ThreeToTwoExpected);

	replaced = Baroque::String::Replace(ThreeToTwoExpected, U'é'_cp, U'à'_cp);
	EXPECT_EQ(replaced.Size(), 25);
	EXPECT_STREQ(replaced.Data(), SameLengthExpected);

	replaced = Baroque::String::Replace("Test A Testing", U'A'_cp, U'😀'_cp);
	EXPECT_EQ(replaced.Size(), 17);
	EXPECT_STREQ(replaced.Data(), OneToFourExpected);
}

TEST(String, StaticReplaceSubstringWithNull)
{
	auto replaced = Baroque::String::Replace("World Hello World", "World", Baroque::StringView{});

	EXPECT_EQ(replaced.Size(), 7);
	EXPECT_STREQ(replaced.Data(), " Hello ");
}

TEST(String, StaticReplaceSubstringWithEmpty)
{
	auto replaced = Baroque::String::Replace("World Hello World", "World", "");

	EXPECT_EQ(replaced.Size(), 7);
	EXPECT_STREQ(replaced.Data(), " Hello ");
}

TEST(String, StaticReplaceSubstringWithSameLengthString)
{
	auto replaced = Baroque::String::Replace("World Hello World", "World", "Monde");

	EXPECT_EQ(replaced.Size(), 17);
	EXPECT_STREQ(replaced.Data(), "Monde Hello Monde");
}

TEST(String, StaticReplaceSubstringWithShorterString)
{
	auto replaced = Baroque::String::Replace("World Hello World", "World", "Te");

	EXPECT_EQ(replaced.Size(), 11);
	EXPECT_STREQ(replaced.Data(), "Te Hello Te");
}

TEST(String, StaticReplaceSubstringWithLongerString)
{
	auto replaced = Baroque::String::Replace("World Hello World", "World", "TerraMondeAlpha");

	EXPECT_EQ(replaced.Size(), 37);
	EXPECT_STREQ(replaced.Data(), "TerraMondeAlpha Hello TerraMondeAlpha");
}

TEST(String, SplitByChar)
{
	const char* Expected[] = {
		"A",
		"Bb",
		"ccc",
		"DDDD",
		"eeee"
	};

	Baroque::String string{ "A/Bb/ccc/DDDD/eeee" };

	auto splitted = string.Split('/');

	EXPECT_EQ(splitted.Size(), 5);

	for (std::size_t i = 0; i < splitted.Size(); ++i)
	{
		EXPECT_EQ(splitted[i], Expected[i]);
	}
}

TEST(String, SplitByCharUsingSmallArray)
{
	const char* Expected[] = {
		"A",
		"Bb",
		"ccc",
		"DDDD",
		"eeee"
	};

	Baroque::String string{ "A/Bb/ccc/DDDD/eeee" };

	Baroque::SmallArray<Baroque::StringView, 8> splitted;

	string.Split('/', splitted);

	EXPECT_EQ(splitted.Size(), 5);

	for (std::size_t i = 0; i < splitted.Size(); ++i)
	{
		EXPECT_EQ(splitted[i], Expected[i]);
	}
}

TEST(String, SplitByCodepoint)
{
	using namespace Baroque::Unicode;

	const char* Expected[] = {
		"A",
		"Bb",
		"ccc",
		"DDDD",
		"eeee"
	};

	Baroque::String string{ u8"A¥Bb¥ccc¥DDDD¥eeee" };

	auto splitted = string.Split(U'¥'_cp);

	EXPECT_EQ(splitted.Size(), 5);

	for (std::size_t i = 0; i < splitted.Size(); ++i)
	{
		EXPECT_EQ(splitted[i], Expected[i]);
	}
}

TEST(String, SplitByCodepointUsingSmallArray)
{
	using namespace Baroque::Unicode;

	const char* Expected[] = {
		"A",
		"Bb",
		"ccc",
		"DDDD",
		"eeee"
	};

	Baroque::String string{ u8"A¥Bb¥ccc¥DDDD¥eeee" };

	Baroque::SmallArray<Baroque::StringView, 8> splitted;

	string.Split(U'¥'_cp, splitted);

	EXPECT_EQ(splitted.Size(), 5);

	for (std::size_t i = 0; i < splitted.Size(); ++i)
	{
		EXPECT_EQ(splitted[i], Expected[i]);
	}
}

TEST(String, JoinByInitializerList)
{
	auto result = Baroque::String::Join(", ", { "A", "bb", "CCC", "DDDD" });

	EXPECT_STREQ(result.Data(), "A, bb, CCC, DDDD");

	auto onlyTwo = Baroque::String::Join(" + ", { "123", "456" });

	EXPECT_STREQ(onlyTwo.Data(), "123 + 456");
}

TEST(String, JoinByArrayView)
{
	Baroque::Array<Baroque::String> array;

	array.Add("This");
	array.Add("is");
	array.Add("an");
	array.Add("array");

	auto result = Baroque::String::Join(" ", array);

	EXPECT_STREQ(result.Data(), "This is an array");

	Baroque::Array<Baroque::StringView> onlyTwoArray;
	onlyTwoArray.Add("123");
	onlyTwoArray.Add("456");

	auto onlyTwoString = Baroque::String::Join(" + ", onlyTwoArray);

	EXPECT_STREQ(onlyTwoString.Data(), "123 + 456");
}

TEST(String, JoinByTemplateParameter)
{
	auto result = Baroque::String::Join(", ", "Z", "xx", "YYY", "VVVV");

	EXPECT_STREQ(result.Data(), "Z, xx, YYY, VVVV");

	auto onlyTwo = Baroque::String::Join(", ", "ABBA", "AC/DC");

	EXPECT_STREQ(onlyTwo.Data(), "ABBA, AC/DC");
}

TEST(String, OperatorPlusTwoString)
{
	Baroque::String left("Left_");
	Baroque::String right("Right!");

	auto result = left + right;

	EXPECT_EQ(result.Size(), 11);
	EXPECT_STREQ(result.Data(), "Left_Right!");
}

TEST(String, OperatorPlusStringViewAndString)
{
	Baroque::StringView view{ "LeftView " };
	Baroque::String string{ "RightString" };

	auto result = view + string;

	EXPECT_EQ(result.Size(), 20);
	EXPECT_STREQ(result.Data(), "LeftView RightString");

	auto withLiteral = "LiteralLeft " + string;

	EXPECT_EQ(withLiteral.Size(), 23);
	EXPECT_STREQ(withLiteral.Data(), "LiteralLeft RightString");
}

TEST(String, OperatorPlusStringAndStringView)
{
	Baroque::String string{ "LeftString " };
	Baroque::StringView view{ "RightView" };

	auto result = string + view;

	EXPECT_EQ(result.Size(), 20);
	EXPECT_STREQ(result.Data(), "LeftString RightView");

	auto withLiteral = string + "LiteralRight";

	EXPECT_EQ(withLiteral.Size(), 23);
	EXPECT_STREQ(withLiteral.Data(), "LeftString LiteralRight");
}

TEST(String, OperatorPlusShouldSupportEmptyString)
{
	const char* Expected = "SingleString";

	Baroque::String emptyString;
	Baroque::String string{ Expected };

	auto leftEmpty = "" + string;
	auto leftNull = Baroque::StringView{} + string;
	auto leftStringEmpty = emptyString + string;

	auto rightEmpty = string + "";
	auto rightNull = string + Baroque::StringView{};
	auto rightStringEmpty = string + emptyString;

	EXPECT_STREQ(leftEmpty.Data(), Expected);
	EXPECT_STREQ(leftNull.Data(), Expected);
	EXPECT_STREQ(leftStringEmpty.Data(), Expected);
	EXPECT_STREQ(rightEmpty.Data(), Expected);
	EXPECT_STREQ(rightNull.Data(), Expected);
	EXPECT_STREQ(rightStringEmpty.Data(), Expected);
}


TEST(String, OperatorPlusEqualsSingleChar)
{
	Baroque::String string;

	EXPECT_TRUE(string.IsShortString());
	EXPECT_EQ(string.Size(), 0);

	string += 'A';

	EXPECT_EQ(string.Size(), 1);
	EXPECT_STREQ(string.Data(), "A");
}

TEST(String, OperatorPlusEqualsAnotherString)
{
	Baroque::String result{ "Hello" };

	result += Baroque::String{ " World" };

	EXPECT_EQ(result.Size(), 11);
	EXPECT_STREQ(result.Data(), "Hello World");
}

TEST(String, OperatorPlusEqualsConstPtr)
{
	Baroque::String result{ "Hello" };

	result += " World";

	EXPECT_EQ(result.Size(), 11);
	EXPECT_STREQ(result.Data(), "Hello World");
}

TEST(String, OperatorPlusEqualsStringView)
{
	Baroque::String result{ "Hello" };

	result += Baroque::StringView{ " World" };

	EXPECT_EQ(result.Size(), 11);
	EXPECT_STREQ(result.Data(), "Hello World");
}

TEST(String, OperatorPlusEqualsShouldMoveToHeapWhenExceedingShortSize)
{
	Baroque::String string("ABCDEFGHIJKLMNOPQRSTUV");

	EXPECT_TRUE(string.IsShortString());

	auto* oldData = string.Data();

	string += "WXYZ0123456789";

	EXPECT_FALSE(string.IsShortString());
	EXPECT_NE(oldData, string.Data());
	EXPECT_STREQ(string.Data(), "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
}

TEST(String, OperatorPlusEqualsCodepoint)
{
	using namespace Baroque::Unicode;

	{
		const char Expected[] = "A";

		Baroque::String string;

		string += U'A'_cp;

		EXPECT_EQ(string.Size(), 1);
		EXPECT_EQ(string.CodepointLength(), 1);

		EXPECT_STREQ(string.Data(), Expected);

		EXPECT_EQ(string.CodepointAt(0), Baroque::Unicode::Codepoint{ 0x0041 });
	}

	{
		const char Expected[] = "\xC3\xA9";

		Baroque::String string;

		string += U'é'_cp;

		EXPECT_EQ(string.Size(), 2);
		EXPECT_EQ(string.CodepointLength(), 1);

		EXPECT_STREQ(string.Data(), Expected);

		EXPECT_EQ(string.CodepointAt(0), Baroque::Unicode::Codepoint{ 0x00E9 });
	}

	{
		const char Expected[] = "\xE9\x9B\xBB";

		Baroque::String string;

		string += U'電'_cp;

		EXPECT_EQ(string.Size(), 3);
		EXPECT_EQ(string.CodepointLength(), 1);

		EXPECT_STREQ(string.Data(), Expected);

		EXPECT_EQ(string.CodepointAt(0), Baroque::Unicode::Codepoint{ 0x96FB });
	}

	{
		const char Expected[] = "\xF0\x9F\x98\x80";

		Baroque::String string;

		string += U'😀'_cp;

		EXPECT_EQ(string.Size(), 4);
		EXPECT_EQ(string.CodepointLength(), 1);

		EXPECT_STREQ(string.Data(), Expected);

		EXPECT_EQ(string.CodepointAt(0), Baroque::Unicode::Codepoint{ 0x1F600 });
	}
}

TEST(String, OperatorEquals)
{
	Baroque::String leftString("LeftString");
	Baroque::String rightString("RightString");
	Baroque::String similarLeftString("LeftString");
	Baroque::String sameSizeButDifferentContent("0123456789");

	EXPECT_FALSE(leftString == rightString);
	EXPECT_TRUE(leftString == leftString);
	EXPECT_TRUE(leftString == similarLeftString);
	EXPECT_FALSE(leftString == sameSizeButDifferentContent);

	EXPECT_FALSE(leftString == "RightString");
	EXPECT_TRUE(leftString == "LeftString");
	EXPECT_FALSE("LeftString" == rightString);
	EXPECT_TRUE("LeftString" == leftString);
}

TEST(String, OperatorNotEquals)
{
	Baroque::String leftString("LeftString");
	Baroque::String rightString("RightString");
	Baroque::String similarLeftString("LeftString");
	Baroque::String sameSizeButDifferentContent("0123456789");

	EXPECT_TRUE(leftString != rightString);
	EXPECT_FALSE(leftString != leftString);
	EXPECT_FALSE(leftString != similarLeftString);
	EXPECT_TRUE(leftString != sameSizeButDifferentContent);

	EXPECT_TRUE(leftString != "RightString");
	EXPECT_FALSE(leftString != "LeftString");
	EXPECT_TRUE("LeftString" != rightString);
	EXPECT_FALSE("LeftString" != leftString);
}

TEST(String, OperatorLess)
{
	Baroque::String a{ "a" };
	Baroque::String b{ "b" };
	Baroque::String bb{ "bb" };

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

TEST(String, OperatorLessOrEquals)
{
	Baroque::String a{ "a" };
	Baroque::String b{ "b" };
	Baroque::String bb{ "bb" };

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

TEST(String, OperatorGreater)
{
	Baroque::String a{ "a" };
	Baroque::String b{ "b" };
	Baroque::String bb{ "bb" };

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

TEST(String, OperatorGreaterOrEquals)
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

TEST(String, StringCustomLiteral)
{
	using namespace Baroque::Literals;

	const char* UnicodeExpected = u8"UTF-8　string　ユニコード文字列";

	Baroque::String asciiString = "ASCII string"_s;
	EXPECT_EQ(asciiString.Size(), 12);
	EXPECT_STREQ(asciiString.Data(), "ASCII string");

	Baroque::String unicodeString = u8"UTF-8　string　ユニコード文字列"_s;
	EXPECT_EQ(unicodeString.Size(), 41);
	EXPECT_STREQ(unicodeString.Data(), UnicodeExpected);
}

TEST(SmallString, ShouldSkipSmallStringOptimisationAndUseSmallCapacity)
{
	{
		Baroque::SmallString<32> smallString;

		EXPECT_EQ(smallString.Size(), 0);
		// Capacity does not include the zero terminator
		// but SmallString should use the stack size fully.
		EXPECT_EQ(smallString.Capacity(), 31);

		void* data = smallString.Data();
		void* rangeStart = &smallString;
		void* rangeEnd = &smallString + (sizeof(smallString) - 24);

		EXPECT_TRUE(data >= rangeStart && data <= rangeEnd);
	}

	{
		Baroque::SmallString<32> smallString(12);

		EXPECT_EQ(smallString.Size(), 12);
		EXPECT_EQ(smallString.Capacity(), 31);

		void* data = smallString.Data();
		void* rangeStart = &smallString;
		void* rangeEnd = &smallString + (sizeof(smallString) - 24);

		EXPECT_TRUE(data >= rangeStart && data <= rangeEnd);
	}

	{
		Baroque::SmallString<32> smallString(TestShortString);

		EXPECT_EQ(smallString.Size(), 5);
		EXPECT_EQ(smallString.Capacity(), 31);

		void* data = smallString.Data();
		void* rangeStart = &smallString;
		void* rangeEnd = &smallString + (sizeof(smallString) - 24);

		EXPECT_TRUE(data >= rangeStart && data <= rangeEnd);
	}

	{
		Baroque::SmallString<32> smallString(TestShortString, 2);

		EXPECT_EQ(smallString.Size(), 2);
		EXPECT_EQ(smallString.Capacity(), 31);

		void* data = smallString.Data();
		void* rangeStart = &smallString;
		void* rangeEnd = &smallString + (sizeof(smallString) - 24);

		EXPECT_TRUE(data >= rangeStart && data <= rangeEnd);
	}

	{
		Baroque::SmallString<64> smallString(Baroque::StringView{ "Hello World!" });

		EXPECT_EQ(smallString.Size(), 12);
		EXPECT_EQ(smallString.Capacity(), 63);

		void* data = smallString.Data();
		void* rangeStart = &smallString;
		void* rangeEnd = &smallString + (sizeof(smallString) - 24);

		EXPECT_TRUE(data >= rangeStart && data <= rangeEnd);
	}

	{
		Baroque::String modifiableString{ "Modifiable String" };

		Baroque::SmallString<64> smallString(modifiableString.ToStringSpan());

		EXPECT_EQ(smallString.Size(), 17);
		EXPECT_EQ(smallString.Capacity(), 63);

		void* data = smallString.Data();
		void* rangeStart = &smallString;
		void* rangeEnd = &smallString + (sizeof(smallString) - 24);

		EXPECT_TRUE(data >= rangeStart && data <= rangeEnd);
	}

	{
		Baroque::SmallString<128> smallString("Constant String");

		EXPECT_EQ(smallString.Size(), 15);
		EXPECT_EQ(smallString.Capacity(), 127);

		void* data = smallString.Data();
		void* rangeStart = &smallString;
		void* rangeEnd = &smallString + (sizeof(smallString) - 24);

		EXPECT_TRUE(data >= rangeStart && data <= rangeEnd);
	}
}

TEST(SmallString, ShouldMoveFromStackToHeap)
{
	Baroque::SmallString<8> smallString{ "0123456" };

	void* data = smallString.Data();
	void* rangeStart = &smallString;
	void* rangeEnd = &smallString + (sizeof(smallString) - 24);

	EXPECT_TRUE(data >= rangeStart && data <= rangeEnd);

	smallString.Append('7');

	data = smallString.Data();

	EXPECT_FALSE(data >= rangeStart && data <= rangeEnd);
}

TEST(SmallString, CopySmallStringToString)
{
	Baroque::SmallString<8> smallString{ "0123456" };
	Baroque::String string(smallString);

	EXPECT_EQ(string, smallString);

	Baroque::String copyAssign;

	copyAssign = smallString;

	EXPECT_EQ(copyAssign, smallString);
}

TEST(SmallString, CopyCtor)
{
	Baroque::SmallString<8> smallString{ "0123456" };
	Baroque::SmallString<8> copy(smallString);

	EXPECT_EQ(smallString, copy);
	EXPECT_EQ(copy.Capacity(), 7);
}

TEST(SmallString, CopyAssignment)
{
	Baroque::SmallString<8> smallString{ "0123456" };
	Baroque::SmallString<8> copy;
	
	copy = smallString;

	EXPECT_EQ(smallString, copy);
	EXPECT_EQ(copy.Capacity(), 7);
}
