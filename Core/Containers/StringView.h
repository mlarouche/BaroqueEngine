#pragma once

#include "Core/CoreDefines.h"

#include "Core/Algorithms/MinMax.h"
#include "Core/Containers/StringSpan.h"
#include "Core/Memory/Memory.h"
#include "Core/Unicode/Codepoint.h"
#include "Core/Unicode/StringFunctions.h"

#include <cstring>

namespace Baroque
{
	class StringView
	{
	public:
		using Value = char;
		using ConstPointer = Value const*;
		using ConstReference = Value const&;
		using SizeType = std::size_t;

		constexpr StringView()
		{}

		constexpr StringView(ConstPointer ptr)
		: _begin(ptr)
		, _end(ptr + Unicode::ByteLength(ptr))
		{
		}

		constexpr StringView(ConstPointer begin, ConstPointer end)
		: _begin(begin)
		, _end(end)
		{}

		constexpr StringView(ConstPointer ptr, SizeType size)
		: _begin(ptr)
		, _end(ptr + size)
		{}

		constexpr StringView(StringSpan span)
		: _begin(span.begin())
		, _end(span.end())
		{
		}

		constexpr StringView(const StringView& copy)
		: _begin(copy._begin)
		, _end(copy._end)
		{
		}

		StringView& operator=(const StringView& copy)
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

		constexpr bool Contains(StringView view) const
		{
			return Find(view) != nullptr;
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

		constexpr bool EndsWith(StringView view) const
		{
			auto viewSize = view.Size();

			if (viewSize > 0 && Size() >= viewSize)
			{
				auto* viewData = view.Data();

				auto* thisIt = end() - viewSize;
				auto* thisItEnd = begin() - 1;

				for (; thisIt != thisItEnd; --thisIt)
				{
					if (*thisIt == viewData[0])
					{
						auto* viewIt = view.begin() + 1;
						auto* viewItEnd = view.end();

						auto* subIt = thisIt + 1;

						for (; viewIt != viewItEnd && subIt != thisItEnd; ++viewIt, ++subIt)
						{
							if (*subIt != *viewIt)
							{
								break;
							}
						}

						if (viewIt == viewItEnd)
						{
							return true;
						}
					}
				}
			}

			return false;
		}

		constexpr ConstPointer Find(Value value) const
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

		constexpr ConstPointer Find(Unicode::Codepoint codepoint) const
		{
			auto codepointIt = CodepointBegin();
			auto codepointItEnd = CodepointEnd();

			for (; codepointIt != codepointItEnd; ++codepointIt)
			{
				if (*codepointIt == codepoint)
				{
					return codepointIt.GetRawIterator();
				}
			}

			return nullptr;
		}

		constexpr ConstPointer Find(StringView view) const
		{
			auto* viewData = view.Data();
			auto viewSize = view.Size();

			auto* thisIt = begin();
			auto* thisItEnd = end();

			if (viewSize > 0 && Size() >= viewSize)
			{
				for (; thisIt != thisItEnd; ++thisIt)
				{
					if (*thisIt == viewData[0])
					{
						auto* viewIt = view.begin() + 1;
						auto* viewItEnd = view.end();

						auto* subIt = thisIt + 1;

						for (; viewIt != viewItEnd && subIt != thisItEnd; ++viewIt, ++subIt)
						{
							if (*subIt != *viewIt)
							{
								break;
							}
						}

						if (viewIt == viewItEnd)
						{
							return thisIt;
						}
					}
				}
			}

			return nullptr;
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

		constexpr SizeType IndexOf(StringView view) const
		{
			auto thisSize = Size();
			auto viewSize = view.Size();

			auto* thisData = Data();
			auto* viewData = view.Data();

			if (viewSize > 0 && thisSize >= viewSize)
			{
				for (SizeType thisIndex = 0; thisIndex < thisSize; ++thisIndex)
				{
					if (thisData[thisIndex] == viewData[0])
					{
						SizeType viewIndex = 1;
						for (; viewIndex < viewSize && (thisIndex + viewIndex) < thisSize; ++viewIndex)
						{
							if (thisData[thisIndex + viewIndex] != viewData[viewIndex])
							{
								break;
							}
						}

						if (viewIndex == viewSize)
						{
							return thisIndex;
						}
					}
				}
			}

			return thisSize;
		}

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

		constexpr SizeType LastIndexOf(StringView view) const
		{
			auto* thisData = Data();
			auto thisSize = Size();
			auto* viewData = view.Data();
			auto viewSize = view.Size();

			if (viewSize > 0 && thisSize >= viewSize)
			{
				for (SizeType thisIndex = thisSize - viewSize; thisIndex >= 0 && thisIndex < thisSize; --thisIndex)
				{
					if (thisData[thisIndex] == viewData[0])
					{
						SizeType viewIndex = 1;

						for (; viewIndex < viewSize && (thisIndex + viewIndex) < thisSize; ++viewIndex)
						{
							if (thisData[thisIndex + viewIndex] != viewData[viewIndex])
							{
								break;
							}
						}

						if (viewIndex == viewSize)
						{
							return thisIndex;
						}
					}
				}
			}

			return thisSize;
		}

		constexpr ConstPointer ReverseFind(Value value) const
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

		constexpr ConstPointer ReverseFind(Unicode::Codepoint codepoint) const
		{
			if (!IsEmpty())
			{
				auto codepointIt = CodepointEnd() - 1;
				auto codepointItEnd = CodepointBegin() - 1;

				for (; codepointIt != codepointItEnd; --codepointIt)
				{
					if (*codepointIt == codepoint)
					{
						return codepointIt.GetRawIterator();
					}
				}
			}

			return nullptr;
		}

		constexpr ConstPointer ReverseFind(StringView view) const
		{
			auto* viewData = view.Data();
			auto viewSize = view.Size();

			if (viewSize > 0 && Size() >= viewSize)
			{
				auto* thisIt = end() - viewSize;
				auto* thisItEnd = begin() - 1;

				for (; thisIt != thisItEnd; --thisIt)
				{
					if (*thisIt == viewData[0])
					{
						auto* viewIt = view.begin() + 1;
						auto* viewItEnd = view.end();

						auto* subIt = thisIt + 1;

						for (; viewIt != viewItEnd && subIt != thisItEnd; ++viewIt, ++subIt)
						{
							if (*subIt != *viewIt)
							{
								break;
							}
						}

						if (viewIt == viewItEnd)
						{
							return thisIt;
						}
					}
				}
			}

			return nullptr;
		}

		constexpr StringView Slice(SizeType start, SizeType end) const
		{
			return StringView{ Data() + start, Data() + ((end - start) + 1) };
		}

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

		constexpr bool StartsWith(StringView view) const
		{
			auto viewSize = view.Size();

			if (viewSize > 0 && Size() >= viewSize)
			{
				auto* viewData = view.Data();

				auto* thisIt = begin();
				auto* thisItEnd = end();

				if (*thisIt == viewData[0])
				{
					auto* viewIt = view.begin() + 1;
					auto* viewItEnd = view.end();

					auto* subIt = thisIt + 1;

					for (; viewIt != viewItEnd && subIt != thisItEnd; ++viewIt, ++subIt)
					{
						if (*subIt != *viewIt)
						{
							break;
						}
					}

					if (viewIt == viewItEnd)
					{
						return true;
					}
				}
			}

			return false;
		}

		constexpr SizeType Size() const
		{
			return _end - _begin;
		}

		constexpr StringView Substring(SizeType start) const
		{
			return StringView{ Data() + start, end() };
		}

		constexpr StringView Substring(SizeType start, SizeType count) const
		{
			return StringView{ Data() + start, count };
		}

		constexpr StringView& TrimBegin()
		{
			auto codepointIt = CodepointBegin();
			auto codepointItEnd = CodepointEnd();

			while (codepointIt != codepointItEnd && codepointIt->IsWhitespace())
			{
				++codepointIt;
			}

			_begin = codepointIt.GetRawIterator();

			return *this;
		}

		constexpr StringView TrimBegin() const
		{
			auto codepointIt = CodepointBegin();
			auto codepointItEnd = CodepointEnd();

			while (codepointIt != codepointItEnd && codepointIt->IsWhitespace())
			{
				++codepointIt;
			}

			return StringView{ codepointIt.GetRawIterator(), end() };
		}

		constexpr StringView& TrimEnd()
		{
			if (!IsEmpty())
			{
				auto codepointIt = CodepointEnd() - 1;
				auto codepointItEnd = CodepointBegin() - 1;

				while (codepointIt != codepointItEnd && codepointIt->IsWhitespace())
				{
					--codepointIt;
				}

				_end = codepointIt.GetRawIterator() + 1;
			}

			return *this;
		}

		constexpr StringView TrimEnd() const
		{
			if (!IsEmpty())
			{
				auto codepointIt = CodepointEnd() - 1;
				auto codepointItEnd = CodepointBegin() - 1;

				while (codepointIt != codepointItEnd && codepointIt->IsWhitespace())
				{
					--codepointIt;
				}

				return StringView{ begin(), codepointIt.GetRawIterator() + 1 };
			}

			return *this;
		}

		constexpr StringView& Trim()
		{
			TrimBegin();
			TrimEnd();

			return *this;
		}

		constexpr StringView Trim() const
		{
			return TrimBegin().TrimEnd();
		}

		StringImplementation<Baroque::Memory::DefaultAllocator> ToString() const;

		template<typename T>
		T ToStringType() const
		{
			return T{ Data(), Size() };
		}

		constexpr ConstPointer begin() const
		{
			return _begin;
		}

		constexpr ConstPointer end() const
		{
			return _end;
		}

		constexpr ConstReference operator[](SizeType index) const
		{
			return Data()[index];
		}

	private:
		ConstPointer _begin = nullptr;
		ConstPointer _end = nullptr;
	};

	inline constexpr bool operator==(StringView left, StringView right)
	{
		auto leftSize = left.Size();
		auto rightSize = right.Size();

		if (leftSize == rightSize)
		{
			return std::memcmp(left.Data(), right.Data(), leftSize) == 0;
		}

		return false;
	}

	inline constexpr bool operator!=(StringView left, StringView right)
	{
		auto leftSize = left.Size();
		auto rightSize = right.Size();

		if (leftSize == rightSize)
		{
			return std::memcmp(left.Data(), right.Data(), leftSize) != 0;
		}

		return true;
	}

	inline bool operator<(StringView left, StringView right)
	{
		auto maxSize = Algorithm::Min(left.Size(), right.Size());

		return std::strncmp(left.Data(), right.Data(), maxSize) < 0;
	}

	inline bool operator<=(StringView left, StringView right)
	{
		auto maxSize = Algorithm::Min(left.Size(), right.Size());

		return std::strncmp(left.Data(), right.Data(), maxSize) <= 0;
	}

	inline bool operator>(StringView left, StringView right)
	{
		auto maxSize = Algorithm::Min(left.Size(), right.Size());

		return std::strncmp(left.Data(), right.Data(), maxSize) > 0;
	}

	inline bool operator>=(StringView left, StringView right)
	{
		auto maxSize = Algorithm::Min(left.Size(), right.Size());

		return std::strncmp(left.Data(), right.Data(), maxSize) >= 0;
	}
}