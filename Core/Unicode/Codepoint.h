#pragma once

#include "Core/CoreDefines.h"

#include "Core/Utilities/StronglyTypeBase.h"
#include "Core/Utilities/TypeTraits.h"

namespace Baroque::Unicode
{
	inline static constexpr auto TwoByteMask = 0b11000000;
	inline static constexpr auto ThreeByteMask = 0b11100000;
	inline static constexpr auto FourByteMask = 0b11110000;
	inline static constexpr auto SequenceByteMask = 0b10000000;

	// Continue to navigate the UTF-8 string even on invalid codepoint
	inline static constexpr std::uint8_t InvalidCodepointValue = 1;

	static constexpr const std::uint8_t CodepointJumpTable[] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		InvalidCodepointValue,
		InvalidCodepointValue,
		InvalidCodepointValue,
		InvalidCodepointValue,
		InvalidCodepointValue,
		InvalidCodepointValue,
		InvalidCodepointValue,
		InvalidCodepointValue, 2, 2, 2, 2, 3, 3, 4, InvalidCodepointValue
	};

	struct Codepoint : public StronglyTypeBase<char32_t, struct CodepointTag>
	{
		constexpr bool IsWhitespace() const
		{
			switch (Value)
			{
				case 0x0009:
				case 0x000A:
				case 0x000B:
				case 0x000C:
				case 0x000D:
				case 0x0020:
				case 0x0085:
				case 0x00A0:
				case 0x1680:
				case 0x2000:
				case 0x2001:
				case 0x2002:
				case 0x2003:
				case 0x2004:
				case 0x2005:
				case 0x2006:
				case 0x2007:
				case 0x2008:
				case 0x2009:
				case 0x200A:
				case 0x2028:
				case 0x2029:
				case 0x202F:
				case 0x205F:
				case 0x3000:
					return true;

				default:
					break;
			}

			return false;
		}

		constexpr std::size_t Length() const
		{
			if (Value < 0x80)
			{
				return 1;
			}
			else if (Value >= 0x80 && Value <= 0x07FF)
			{
				return 2;
			}
			else if (Value >= 0x800 && Value <= 0xFFFF)
			{
				return 3;
			}
			else
			{
				return 4;
			}
		}

		constexpr Codepoint* operator->()
		{
			return this;
		}

		constexpr Codepoint const* operator->() const
		{
			return this;
		}
	};

	constexpr Codepoint operator""_cp(char32_t cp)
	{
		return Codepoint{ cp };
	}

	class CodepointIterator
	{
	public:
		using RawIterator = const char*;

		constexpr CodepointIterator(RawIterator ptr)
			: _it(ptr)
		{}

		constexpr Codepoint operator*() const
		{
			Unicode::Codepoint result{ 0 };

			std::uint8_t firstByte = *_it;
			if (firstByte < 0x80)
			{
				result.Value = firstByte;
			}
			else if ((firstByte & 0b11100000) == Unicode::TwoByteMask)
			{
				result.Value = ((firstByte & 0x1F) << 6)
					| *(_it + 1) & 0x3F
					;
			}
			else if ((firstByte & 0b11110000) == Unicode::ThreeByteMask)
			{
				result.Value = ((firstByte & 0x0F) << 12)
					| (*(_it + 1) & 0x3F) << 6
					| (*(_it + 2) & 0x3F)
					;
			}
			else if ((firstByte & 0b11111000) == Unicode::FourByteMask)
			{
				result.Value = ((firstByte & 0x07) << 18)
					| (*(_it + 1) & 0x3F) << 12
					| (*(_it + 2) & 0x3F) << 6
					| (*(_it + 3) & 0x3F)
					;
			}

			return result;
		}

		constexpr Codepoint operator->() const
		{
			return operator*();
		}

		constexpr CodepointIterator& operator++()
		{
			_it += CodepointJumpTable[static_cast<std::uint8_t>(*_it) >> 3];
			return *this;
		}

		constexpr CodepointIterator& operator--()
		{
			do
			{
				--_it;
			} while ((static_cast<std::uint8_t>(*_it) & 0b11000000) == SequenceByteMask);

			return *this;
		}

		constexpr CodepointIterator operator++(int)
		{
			CodepointIterator copy = *this;
			operator++();
			return copy;
		}

		constexpr CodepointIterator operator--(int)
		{
			CodepointIterator copy = *this;
			operator--();
			return copy;
		}

		constexpr CodepointIterator& operator=(const CodepointIterator& copy)
		{
			_it = copy._it;
			return *this;
		}

		constexpr RawIterator GetRawIterator() const
		{
			return _it;
		}

		constexpr CodepointIterator& operator+(int displacement)
		{
			if (displacement > 0)
			{
				for (int i = 0; i < std::abs(displacement); ++i)
				{
					operator++();
				}
			}
			else if (displacement < 0)
			{
				for (int i = 0; i < std::abs(displacement); ++i)
				{
					operator--();
				}
			}
			

			return *this;
		}

		constexpr CodepointIterator& operator-(int displacement)
		{
			if (displacement > 0)
			{
				for (int i = 0; i < std::abs(displacement); ++i)
				{
					operator--();
				}
			}
			else if (displacement < 0)
			{
				for (int i = 0; i < std::abs(displacement); ++i)
				{
					operator++();
				}
			}

			return *this;
		}

		static std::size_t Distance(const CodepointIterator& left, const CodepointIterator& right)
		{
			std::size_t size = 0;

			auto it = left.GetRawIterator();
			auto itEnd = right.GetRawIterator();

			while (it != itEnd)
			{
				++size;

				it += Unicode::CodepointJumpTable[static_cast<std::uint8_t>(*it) >> 3];
			}

			return size;
		}

		friend constexpr bool operator==(const CodepointIterator&, const CodepointIterator&);
		friend constexpr bool operator!=(const CodepointIterator&, const CodepointIterator&);
		friend constexpr bool operator<(const CodepointIterator&, const CodepointIterator&);
		friend constexpr bool operator<=(const CodepointIterator&, const CodepointIterator&);
		friend constexpr bool operator>(const CodepointIterator&, const CodepointIterator&);
		friend constexpr bool operator>=(const CodepointIterator&, const CodepointIterator&);
		friend constexpr CodepointIterator operator+(const CodepointIterator&, const CodepointIterator&);
		friend constexpr CodepointIterator operator-(const CodepointIterator&, const CodepointIterator&);

	private:
		RawIterator _it;
	};

	inline constexpr bool operator==(const CodepointIterator& left, const CodepointIterator& right)
	{
		return left._it == right._it;
	}
	
	inline constexpr bool operator!=(const CodepointIterator& left, const CodepointIterator& right)
	{
		return left._it != right._it;
	}

	inline constexpr bool operator<(const CodepointIterator& left, const CodepointIterator& right)
	{
		return left._it < right._it;
	}

	inline constexpr bool operator<=(const CodepointIterator& left, const CodepointIterator& right)
	{
		return left._it <= right._it;
	}

	inline constexpr bool operator>(const CodepointIterator& left, const CodepointIterator& right)
	{
		return left._it > right._it;
	}

	inline constexpr bool operator>=(const CodepointIterator& left, const CodepointIterator& right)
	{
		return left._it >= right._it;
	}

	inline constexpr CodepointIterator operator+(const CodepointIterator& left, const CodepointIterator& right)
	{
		return CodepointIterator{ left._it + (right._it - left._it) };
	}

	inline constexpr CodepointIterator operator-(const CodepointIterator& left, const CodepointIterator& right)
	{
		return CodepointIterator{ left._it - (right._it - left._it) };
	}

	class CodepointVisitor
	{
	public:
		using Iterator = CodepointIterator;
		using RawIterator = const char*;

		constexpr CodepointVisitor(RawIterator begin, RawIterator end)
		: _begin(begin)
		, _end(end)
		{
		}

		constexpr Iterator begin() const
		{
			return Iterator(_begin);
		}

		constexpr Iterator end() const
		{
			return Iterator(_end);
		}

	private:
		RawIterator _begin;
		RawIterator _end;
	};
}
