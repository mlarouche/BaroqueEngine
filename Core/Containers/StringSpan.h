#pragma once

#include "Core/CoreDefines.h"

#include "Core/Memory/Memory.h"
#include "Core/Unicode/StringFunctions.h"

namespace Baroque
{
	template<typename Allocator>
	class StringImplementation;

	class StringView;

	class StringSpan
	{
	public:
		using Value = char;
		using Pointer = Value *;
		using ConstPointer = Value const*;
		using Reference = Value &;
		using ConstReference = Value const&;
		using SizeType = std::size_t;

		constexpr StringSpan()
		{}

		constexpr explicit StringSpan(Pointer ptr)
		: _begin(ptr)
		, _end(ptr + Unicode::ByteLength(ptr))
		{}

		constexpr explicit StringSpan(Pointer begin, Pointer end)
		: _begin(begin)
		, _end(end)
		{}

		constexpr explicit StringSpan(Pointer ptr, SizeType size)
		: _begin(ptr)
		, _end(ptr + size)
		{}

		constexpr StringSpan(const StringSpan& copy)
		: _begin(copy._begin)
		, _end(copy._end)
		{}

		StringSpan& operator=(const StringSpan& copy)
		{
			_begin = copy._begin;
			_end = copy._end;
			return *this;
		}

		constexpr Unicode::CodepointVisitor ByCodepoint() const
		{
			return Unicode::CodepointVisitor{ begin(), end() };
		}

		constexpr Unicode::Codepoint CodepointAt(SizeType index) const
		{
			auto it = CodepointBegin();
			auto itEnd = CodepointEnd();

			for (std::size_t i = 0; i <= index && it != itEnd; ++i, ++it)
			{
				if (i == index)
				{
					return *it;
				}
			}

			return Unicode::Codepoint{};
		}

		constexpr Unicode::CodepointIterator CodepointBegin() const
		{
			return Unicode::CodepointIterator{ begin() };
		}

		constexpr Unicode::CodepointIterator CodepointEnd() const
		{
			return Unicode::CodepointIterator{ end() };
		}

		constexpr SizeType CodepointLength() const
		{
			return Unicode::CodepointLength(Data());
		}

		constexpr bool Contains(Value value) const
		{
			return Find(value) != nullptr;
		}

		constexpr bool Contains(Unicode::Codepoint codepoint) const
		{
			return Find(codepoint) != nullptr;
		}

		bool Contains(StringView view) const;

		constexpr Pointer Data()
		{
			return _begin;
		}

		constexpr ConstPointer Data() const
		{
			return _begin;
		}

		constexpr bool EndsWith(Value value) const
		{
			if (Size() > 0 && Last() == value)
			{
				return true;
			}

			return false;
		}

		constexpr bool EndsWith(Unicode::Codepoint codepoint) const
		{
			if (Size() > 0 && *(CodepointEnd() - 1) == codepoint)
			{
				return true;
			}

			return false;
		}

		bool EndsWith(StringView view) const;

		constexpr Pointer Find(Value value)
		{
			auto* it = begin();
			auto* itEnd = end();

			for (; it != itEnd; ++it)
			{
				if (*it == value)
				{
					return it;
				}
			}

			return nullptr;
		}

		constexpr ConstPointer Find(Value value) const
		{
			return const_cast<ConstPointer>(const_cast<StringSpan*>(this)->Find(value));
		}

		constexpr Pointer Find(Unicode::Codepoint codepoint)
		{
			auto codepointIt = CodepointBegin();
			auto codepointItEnd = CodepointEnd();

			for (; codepointIt != codepointItEnd; ++codepointIt)
			{
				if (*codepointIt == codepoint)
				{
					return const_cast<Pointer>(codepointIt.GetRawIterator());
				}
			}

			return nullptr;
		}

		constexpr ConstPointer Find(Unicode::Codepoint codepoint) const
		{
			return const_cast<ConstPointer>(const_cast<StringSpan*>(this)->Find(codepoint));
		}

		Pointer Find(StringView view);
		ConstPointer Find(StringView view) const;

		constexpr Reference Front()
		{
			return *begin();
		}

		constexpr ConstReference Front() const
		{
			return *begin();
		}

		constexpr SizeType IndexOf(Value value) const
		{
			auto* data = Data();
			auto size = Size();

			for (SizeType index = 0; index < size; ++index)
			{
				if (data[index] == value)
				{
					return index;
				}
			}

			return size;
		}

		constexpr SizeType IndexOf(Unicode::Codepoint codepoint) const
		{
			auto codepointIt = CodepointBegin();
			auto codepointItEnd = CodepointEnd();

			for (; codepointIt != codepointItEnd; ++codepointIt)
			{
				if (*codepointIt == codepoint)
				{
					return codepointIt.GetRawIterator() - Data();
				}
			}

			return Size();
		}

		SizeType IndexOf(StringView view) const;

		constexpr bool IsEmpty() const
		{
			return _begin == _end;
		}

		constexpr bool IsNull() const
		{
			return _begin == nullptr
				&& _end == nullptr
				;
		}

		constexpr Reference Last()
		{
			return *(end() - 1);
		}

		constexpr ConstReference Last() const
		{
			return *(end() - 1);
		}

		constexpr SizeType LastIndexOf(Value value) const
		{
			auto* data = Data();
			auto size = Size();

			for (SizeType index = size - 1; index >= 0 && index < size; --index)
			{
				if (data[index] == value)
				{
					return index;
				}
			}

			return size;
		}

		constexpr SizeType LastIndexOf(Unicode::Codepoint codepoint) const
		{
			auto size = Size();

			if (size > 0)
			{
				auto codepointIt = CodepointEnd() - 1;
				auto codepointItEnd = CodepointBegin() - 1;

				for (; codepointIt != codepointItEnd; --codepointIt)
				{
					if (*codepointIt == codepoint)
					{
						return codepointIt.GetRawIterator() - Data();
					}
				}
			}

			return size;
		}

		SizeType LastIndexOf(StringView view) const;

		constexpr ConstPointer ReverseFind(Value value)
		{
			auto* it = end() - 1;
			auto* itEnd = begin() - 1;

			for (; it != itEnd; --it)
			{
				if (*it == value)
				{
					return it;
				}
			}

			return nullptr;
		}

		constexpr ConstPointer ReverseFind(Value value) const
		{
			return const_cast<ConstPointer>(const_cast<StringSpan*>(this)->ReverseFind(value));
		}

		constexpr Pointer ReverseFind(Unicode::Codepoint codepoint)
		{
			if (!IsEmpty())
			{
				auto codepointIt = CodepointEnd() - 1;
				auto codepointItEnd = CodepointBegin() - 1;

				for (; codepointIt != codepointItEnd; --codepointIt)
				{
					if (*codepointIt == codepoint)
					{
						return const_cast<Pointer>(codepointIt.GetRawIterator());
					}
				}
			}

			return nullptr;
		}

		constexpr ConstPointer ReverseFind(Unicode::Codepoint codepoint) const
		{
			return const_cast<ConstPointer>(const_cast<StringSpan*>(this)->ReverseFind(codepoint));
		}

		Pointer ReverseFind(StringView view);
		ConstPointer ReverseFind(StringView view) const;

		constexpr StringSpan Slice(SizeType start, SizeType end)
		{
			return StringSpan{ Data() + start, Data() + ((end - start) + 1) };
		}

		StringView Slice(SizeType start, SizeType end) const;

		constexpr bool StartsWith(Value value) const
		{
			if (Size() > 0 && *begin() == value)
			{
				return true;
			}

			return false;
		}

		constexpr bool StartsWith(Unicode::Codepoint codepoint) const
		{
			if (Size() > 0 && *CodepointBegin() == codepoint)
			{
				return true;
			}

			return false;
		}

		bool StartsWith(StringView view) const;

		constexpr SizeType Size() const
		{
			return _end - _begin;
		}

		constexpr StringSpan Substring(SizeType start)
		{
			return StringSpan{ Data() + start, end() };
		}

		StringView Substring(SizeType start) const;

		constexpr StringSpan Substring(SizeType start, SizeType count)
		{
			return StringSpan{ Data() + start, count };
		}

		StringView Substring(SizeType start, SizeType count) const;

		constexpr StringSpan& TrimBegin()
		{
			auto codepointIt = CodepointBegin();
			auto codepointItEnd = CodepointEnd();

			while (codepointIt != codepointItEnd && codepointIt->IsWhitespace())
			{
				++codepointIt;
			}

			_begin = const_cast<Pointer>(codepointIt.GetRawIterator());

			return *this;
		}

		constexpr StringSpan TrimBegin() const
		{
			auto codepointIt = CodepointBegin();
			auto codepointItEnd = CodepointEnd();

			while (codepointIt != codepointItEnd && codepointIt->IsWhitespace())
			{
				++codepointIt;
			}

			return StringSpan { const_cast<Pointer>(codepointIt.GetRawIterator()), _end };
		}

		constexpr StringSpan& TrimEnd()
		{
			if (!IsEmpty())
			{
				auto codepointIt = CodepointEnd() - 1;
				auto codepointItEnd = CodepointBegin() - 1;

				while (codepointIt != codepointItEnd && codepointIt->IsWhitespace())
				{
					--codepointIt;
				}

				_end = const_cast<Pointer>(codepointIt.GetRawIterator()) + 1;
			}

			return *this;
		}

		constexpr StringSpan TrimEnd() const
		{
			if (!IsEmpty())
			{
				auto codepointIt = CodepointEnd() - 1;
				auto codepointItEnd = CodepointBegin() - 1;

				while (codepointIt != codepointItEnd && codepointIt->IsWhitespace())
				{
					--codepointIt;
				}

				return StringSpan { _begin, const_cast<Pointer>(codepointIt.GetRawIterator()) + 1 };
			}

			return *this;
		}

		constexpr StringSpan& Trim()
		{
			TrimBegin();
			TrimEnd();

			return *this;
		}

		constexpr StringSpan Trim() const
		{
			return TrimBegin().TrimEnd();
		}

		StringImplementation<Baroque::Memory::DefaultAllocator> ToString() const;

		template<typename T>
		T ToStringType()
		{
			return T{ Data(), Size() };
		}

		constexpr Pointer begin()
		{
			return _begin;
		}

		constexpr ConstPointer begin() const
		{
			return _begin;
		}

		constexpr Pointer end()
		{
			return _end;
		}

		constexpr ConstPointer end() const
		{
			return _end;
		}

		constexpr Reference operator[](SizeType index)
		{
			return Data()[index];
		}

		constexpr ConstReference operator[](SizeType index) const
		{
			return Data()[index];
		}

	private:
		Pointer _begin = nullptr;
		Pointer _end = nullptr;
	};
}