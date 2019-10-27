#pragma once

#include "Core/CoreDefines.h"

#include "Core/Algorithms/MinMax.h"
#include "Core/Containers/Array.h"
#include "Core/Containers/StringSpan.h"
#include "Core/Containers/StringView.h"
#include "Core/Memory/Memory.h"
#include "Core/Unicode/Codepoint.h"
#include "Core/Unicode/StringFunctions.h"

#include <cstring>

namespace Baroque
{
	BAROQUE_EXTERN_MEMORY_CATEGORY(String)

	template<typename Allocator>
	class StringImplementation : private Allocator
	{
	private:
		static constexpr auto GrowFactor = 2.0;

		static constexpr auto ShortMask = 0x80;
		static constexpr auto HeapMask = ~(std::size_t(~0) >> 1);

		static constexpr bool IsUsingStackCapacity = (Allocator::StackCapacity > 0);

	public:
		using Value = char;
		using Pointer = Value *;
		using ConstPointer = Value const*;
		using Reference = Value &;
		using ConstReference = Value const&;
		using MoveReference = Value &&;
		using SizeType = std::size_t;

		constexpr StringImplementation()
		{
			zeroInit();

			if constexpr (IsUsingStackCapacity)
			{
				reallocate(Allocator::StackCapacity - 1);
			}

			setSize(0);
		}

		constexpr explicit StringImplementation(SizeType size)
		{
			zeroInit();

			if constexpr (IsUsingStackCapacity)
			{
				reallocate(Allocator::StackCapacity - 1);
			}

			Resize(size);
		}

		constexpr StringImplementation(ConstPointer value)
		{
			zeroInit();

			if constexpr (IsUsingStackCapacity)
			{
				reallocate(Allocator::StackCapacity - 1);
			}

			assign(value, Unicode::ByteLength(value));
		}

		constexpr StringImplementation(ConstPointer value, SizeType size)
		{
			zeroInit();

			if constexpr (IsUsingStackCapacity)
			{
				reallocate(Allocator::StackCapacity - 1);
			}

			assign(value, size);
		}

		explicit constexpr StringImplementation(StringView view)
		{
			zeroInit();

			if constexpr (IsUsingStackCapacity)
			{
				reallocate(Allocator::StackCapacity - 1);
			}

			assign(view.Data(), view.Size());
		}

		constexpr StringImplementation(const StringImplementation& copy)
		{
			zeroInit();

			if constexpr (IsUsingStackCapacity)
			{
				reallocate(Algorithm::Max(Allocator::StackCapacity - 1, copy.Size()));
			}

			assign(copy.Data(), copy.Size());
		}

		template<typename = std::enable_if_t<!IsUsingStackCapacity>>
		constexpr StringImplementation(StringImplementation&& move)
		{
			zeroInit();

			if (move.IsShortString())
			{
				std::memcpy(Data(), move.Data(), move.Size());
				Short.InvertedSize = move.Short.InvertedSize;
			}
			else
			{
				Heap.Capacity = move.Heap.Capacity;
				Heap.Size = move.Heap.Size;
				Heap.Data = move.Heap.Data;
			}

			move.zeroInit();
			move.setSize(0);
		}

		~StringImplementation()
		{
			internalDestructor();
		}

		constexpr StringImplementation& operator=(const StringImplementation& copy)
		{
			if (this != &copy)
			{
				internalDestructor();

				zeroInit();

				if constexpr (IsUsingStackCapacity)
				{
					reallocate(Algorithm::Max(Allocator::StackCapacity - 1, copy.Size()));
				}

				assign(copy.Data(), copy.Size());
			}

			return *this;
		}

		template<typename = std::enable_if_t<!IsUsingStackCapacity>>
		constexpr StringImplementation& operator=(StringImplementation&& move)
		{
			internalDestructor();

			zeroInit();

			if (move.IsShortString())
			{
				std::memcpy(Data(), move.Data(), move.Size());
				Short.InvertedSize = move.Short.InvertedSize;
			}
			else
			{
				Heap.Capacity = move.Heap.Capacity;
				Heap.Size = move.Heap.Size;
				Heap.Data = move.Heap.Data;
			}

			move.zeroInit();
			move.setSize(0);

			return *this;
		}

		// Required to disambiguate between copy-assignment and StringView assignment
		constexpr StringImplementation& operator=(ConstPointer value)
		{
			internalDestructor();

			zeroInit();
			assign(value, Unicode::ByteLength(value));

			return *this;
		}

		// StringView needs to be a const reference to remove ambiguity
		constexpr StringImplementation& operator=(const StringView& view)
		{
			internalDestructor();

			zeroInit();
			assign(view.Data(), view.Size());

			return *this;
		}

		constexpr void Append(Value value)
		{
			ensureCapacity();

			auto size = Size();

			Data()[size] = value;

			setSize(size + 1);
		}

		constexpr void Append(Unicode::Codepoint codepoint)
		{
			auto codepointLength = codepoint.Length();

			auto size = Size();
			auto newSize = size + codepointLength;

			ensureCapacity(newSize);

			insertCodepoint(Data() + size, codepoint, codepointLength);

			setSize(newSize);
		}

		constexpr void Append(StringView view)
		{
			auto thisSize = Size();
			auto viewSize = view.Size();
			auto newSize = thisSize + viewSize;

			ensureCapacity(newSize);

			std::memcpy(Data() + thisSize, view.Data(), viewSize);

			setSize(newSize);
		}

		constexpr Unicode::CodepointVisitor ByCodepoint() const
		{
			return Unicode::CodepointVisitor{ begin(), end() };
		}

		constexpr SizeType Capacity() const
		{
			if (IsShortString())
			{
				return sizeof(Short.Data);
			}

			return Heap.Capacity & SizeType(~HeapMask);
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

		constexpr void Clear()
		{
			std::memset(Data(), 0, Size());
			setSize(0);
		}

		constexpr Pointer Data()
		{
			if (IsShortString())
			{
				return Short.Data;
			}

			return Heap.Data;
		}

		constexpr ConstPointer Data() const
		{
			if (IsShortString())
			{
				return Short.Data;
			}

			return Heap.Data;
		}

		constexpr Pointer Erase(ConstPointer it)
		{
			RemoveAt(it - Data());
			return const_cast<Pointer>(it);
		}

		constexpr Unicode::CodepointIterator Erase(Unicode::CodepointIterator eraseIt)
		{
			removeCodepointIterator(eraseIt);
			return eraseIt;
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

		constexpr Reference Front()
		{
			return *begin();
		}

		constexpr ConstReference Front() const
		{
			return *begin();
		}

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
			return const_cast<ConstPointer>(const_cast<StringImplementation*>(this)->Find(value));
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
			return const_cast<ConstPointer>(const_cast<StringImplementation*>(this)->Find(codepoint));
		}

		constexpr Pointer Find(StringView view)
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

		constexpr ConstPointer Find(StringView view) const
		{
			return const_cast<ConstPointer>(const_cast<StringImplementation*>(this)->Find(view));
		}

		static StringImplementation Join(StringView pattern, std::initializer_list<StringView> initList)
		{
			StringImplementation result;

			if (initList.begin() != initList.end())
			{
				auto it = initList.begin();
				auto itEnd = initList.end() - 1;

				for (; it != itEnd; ++it)
				{
					result.Append(*it);
					result.Append(pattern);
				}
				result.Append(*itEnd);
			}

			return result;
		}

		static StringImplementation Join(StringView pattern, ArrayView<StringView> arrayView)
		{
			StringImplementation result;

			if (arrayView.begin() != arrayView.end())
			{
				auto it = arrayView.begin();
				auto itEnd = arrayView.end() - 1;

				for (; it != itEnd; ++it)
				{
					result.Append(*it);
					result.Append(pattern);
				}
				result.Append(*itEnd);
			}

			return result;
		}

		static StringImplementation Join(StringView pattern, ArrayView<StringImplementation> arrayView)
		{
			StringImplementation result;

			if (arrayView.begin() != arrayView.end())
			{
				auto it = arrayView.begin();
				auto itEnd = arrayView.end() - 1;

				for (; it != itEnd; ++it)
				{
					result.Append(*it);
					result.Append(pattern);
				}
				result.Append(*itEnd);
			}

			return result;
		}

		template<typename...Args>
		static StringImplementation Join(StringView pattern, StringView first, Args&&... args)
		{
			return StringImplementation(first) + (... + (StringImplementation(pattern) + StringImplementation(std::forward<Args>(args))));
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
			return Size() == 0;
		}

		constexpr bool IsShortString() const
		{
			return !isHeap();
		}

		constexpr void Insert(SizeType index, Value value)
		{
			auto size = Size();

			ensureCapacity();

			auto* data = Data();

			moveRight(data, index, size, 1);

			data[index] = value;

			setSize(size + 1);
		}

		constexpr void Insert(SizeType index, Unicode::Codepoint codepoint)
		{
			auto size = Size();
			auto codepointLength = codepoint.Length();
			auto newSize = size + codepointLength;

			ensureCapacity(newSize);

			auto* data = Data();
			moveRight(data, index, size, codepointLength);
			insertCodepoint(data + index, codepoint, codepointLength);

			setSize(newSize);
		}

		constexpr void Insert(SizeType index, StringView view)
		{
			auto thisSize = Size();
			auto viewSize = view.Size();
			auto newSize = thisSize + viewSize;

			ensureCapacity(newSize);

			auto* data = Data();
			moveRight(data, index, thisSize, viewSize);
			std::memcpy(data + index, view.Data(), viewSize);

			setSize(newSize);
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

		constexpr SizeType Remove(Value value)
		{
			SizeType removed = 0;

			auto* data = Data();

			auto size = Size();

			for (SizeType index = size - 1; index >= 0 && index < size; --index)
			{
				if (data[index] == value)
				{
					moveLeft(data, index, size, 1);

					--size;
					++removed;

					data[size] = '\0';
				}
			}

			setSize(size);

			return removed;
		}

		constexpr SizeType Remove(Unicode::Codepoint codepointToRemove)
		{
			SizeType removed = 0;

			auto codepointLength = codepointToRemove.Length();

			auto* data = Data();

			auto size = Size();

			auto codepointIt = CodepointBegin();
			auto codepointItEnd = CodepointEnd();

			while (codepointIt != codepointItEnd)
			{
				if (*codepointIt == codepointToRemove)
				{
					moveLeft(data, codepointIt.GetRawIterator() - data, size, codepointLength);

					size -= codepointLength;

					++removed;

					data[size] = '\0';

					codepointIt = codepointIt.GetRawIterator() + codepointLength;
					codepointItEnd = data + size;
				}
				else
				{
					++codepointIt;
				}
			}

			setSize(size);

			return removed;
		}

		constexpr void RemoveAt(SizeType index)
		{
			auto size = Size();
			auto data = Data();

			moveLeft(data, index, size, 1);
			--size;
			data[size] = '\0';

			setSize(size);
		}

		constexpr void RemoveAtCodepoint(SizeType index)
		{
			auto codepointIt = CodepointBegin();
			auto codepointItEnd = CodepointEnd();

			for (std::size_t i = 0; codepointIt != codepointItEnd; ++i, ++codepointIt)
			{
				if (i == index)
				{
					removeCodepointIterator(codepointIt);
					return;
				}
			}
		}

		constexpr StringImplementation& Replace(Value from, Value to)
		{
			auto* it = begin();
			auto* itEnd = end();

			for (; it != itEnd; ++it)
			{
				if (*it == from)
				{
					*it = to;
				}
			}

			return *this;
		}

		static StringImplementation Replace(StringView view, Value from, Value to)
		{
			auto result = view.ToStringType<StringImplementation>();
			result.Replace(from, to);
			return result;
		}

		constexpr StringImplementation& Replace(Unicode::Codepoint from, Unicode::Codepoint to)
		{
			auto codepointIt = CodepointBegin();

			for (; codepointIt != CodepointEnd(); ++codepointIt)
			{
				if (*codepointIt == from)
				{
 					auto fromLength = from.Length();
					auto toLength = to.Length();

					auto size = Size();
					auto newSize = (size - fromLength) + toLength;

					if (fromLength == toLength)
					{
						insertCodepoint(const_cast<Pointer>(codepointIt.GetRawIterator()), to, toLength);
					}
					else if (fromLength > toLength)
					{
						auto* data = Data();

						moveLeft(data, (codepointIt.GetRawIterator() - data) + (fromLength - toLength), size, 1);

						insertCodepoint(const_cast<Pointer>(codepointIt.GetRawIterator()), to, toLength);

						data[newSize] = '\0';
						setSize(newSize);
					}
					else if (fromLength < toLength)
					{
						auto index = (codepointIt.GetRawIterator() - Data());

						ensureCapacity(newSize);

						// ensureCapacity() can invalidate data and iterator, refresh them
						auto* data = Data();
						codepointIt = data + index;
					
						moveRight(data, (codepointIt.GetRawIterator() - data), size, toLength - fromLength);

						insertCodepoint(const_cast<Pointer>(codepointIt.GetRawIterator()), to, toLength);

						data[newSize] = '\0';
						setSize(newSize);
					}
				}
			}

			return *this;
		}

		static StringImplementation Replace(StringView view, Unicode::Codepoint from, Unicode::Codepoint to)
		{
			auto result = view.ToStringType<StringImplementation>();
			result.Replace(from, to);
			return result;
		}

		constexpr StringImplementation& Replace(StringView from, StringView to)
		{
			auto fromSize = from.Size();

			if (fromSize > 0)
			{
				auto* fromData = from.begin();

				auto* toData = to.Data();
				auto toSize = to.Size();

				auto* thisIt = begin();
				auto* thisItEnd = end();

				while (thisIt != thisItEnd)
				{
					if (*thisIt == fromData[0])
					{
						auto* subIt = thisIt + 1;
						auto* subItEnd = end();

						auto* fromIt = fromData + 1;
						auto* fromItEnd = from.end();

						for( ; subIt != subItEnd && fromIt != fromItEnd; ++subIt, ++fromIt)
						{
							if (*subIt != *fromIt)
							{
								break;
							}
						}

						if (fromIt == fromItEnd)
						{
							if (fromSize == toSize)
							{
								std::memcpy(thisIt, toData, toSize);
							}
							else if (fromSize > toSize)
							{
								auto* data = Data();
								auto size = Size();
								auto newSize = (size - fromSize) + toSize;

								moveLeft(data, thisIt - data, size, fromSize - toSize);

								std::memcpy(thisIt, toData, toSize);

								data[newSize] = '\0';
								setSize(newSize);

								thisItEnd = end();
							}
							else if (fromSize < toSize)
							{
								auto size = Size();
								auto newSize = (size - fromSize) + toSize;
								auto index = thisIt - Data();

								ensureCapacity(newSize);

								// ensureCapacity() can invalidate Data() and thisIt
								auto* data = Data();
								thisIt = data + index;

								moveRight(data, thisIt - data, size, toSize - fromSize);

								std::memcpy(thisIt, toData, toSize);

								data[newSize] = '\0';
								setSize(newSize);

								thisItEnd = end();
							}
						}
						else
						{
							++thisIt;
						}
					}
					else
					{
						++thisIt;
					}
				}
			}

			return *this;
		}

		static StringImplementation Replace(StringView view, StringView from, StringView to)
		{
			auto result = view.ToStringType<StringImplementation>();
			result.Replace(from, to);
			return result;
		}

		constexpr void Reserve(SizeType newCapacity)
		{
			auto isGreaterThanShortData = IsUsingStackCapacity ? true : newCapacity > sizeof(Short.Data);

			if (isGreaterThanShortData && newCapacity > Capacity())
			{
				reallocate(newCapacity);
			}
		}

		constexpr void Resize(SizeType newSize)
		{
			auto currentSize = Size();

			if (currentSize < newSize)
			{
				ensureCapacity(newSize);

				Data()[newSize] = '\0';

				setSize(newSize);
			}
			else if (currentSize > newSize)
			{
				std::memset(Data() + newSize, 0, currentSize - newSize);
				setSize(newSize);
			}
		}

		constexpr Pointer ReverseFind(Value value)
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
			return const_cast<ConstPointer>(const_cast<StringImplementation*>(this)->ReverseFind(value));
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
			return const_cast<ConstPointer>(const_cast<StringImplementation*>(this)->ReverseFind(codepoint));
		}

		constexpr Pointer ReverseFind(StringView view)
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

		constexpr ConstPointer ReverseFind(StringView view) const
		{
			return const_cast<ConstPointer>(const_cast<StringImplementation*>(this)->ReverseFind(view));
		}

		constexpr StringSpan Slice(SizeType start, SizeType end)
		{
			return StringSpan{ Data() + start, Data() + ((end - start) + 1) };
		}

		constexpr StringView Slice(SizeType start, SizeType end) const
		{
			return StringView{ Data() + start, Data() + ((end - start) + 1) };
		}

		constexpr Array<StringView> Split(Value value) const
		{
			Array<StringView> result;
			Split(value, result);
			return result;
		}

		template<typename Allocator>
		constexpr void Split(Value value, ArrayImplementation<StringView, Allocator>& outResult) const
		{
			outResult.Clear();

			auto* it = begin();
			auto* itEnd = end();

			auto* previousIt = it;

			for (; it != itEnd; ++it)
			{
				if (*it == value)
				{
					outResult.Emplace(previousIt, it);

					previousIt = it + 1;
				}
			}

			if (previousIt != itEnd)
			{
				outResult.Emplace(previousIt, itEnd);
			}
		}

		constexpr Array<StringView> Split(Unicode::Codepoint codepoint) const
		{
			Array<StringView> result;
			Split(codepoint, result);
			return result;
		}

		template<typename Allocator>
		constexpr void Split(Unicode::Codepoint codepoint, ArrayImplementation<StringView, Allocator>& outResult) const
		{
			outResult.Clear();

			auto codepointIt = CodepointBegin();
			auto codepointItEnd = CodepointEnd();

			auto previousIt = codepointIt;

			for (; codepointIt != codepointItEnd; ++codepointIt)
			{
				if (*codepointIt == codepoint)
				{
					outResult.Emplace(previousIt.GetRawIterator(), codepointIt.GetRawIterator());

					previousIt = codepointIt + 1;
				}
			}

			if (previousIt != codepointItEnd)
			{
				outResult.Emplace(previousIt.GetRawIterator(), codepointItEnd.GetRawIterator());
			}
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
			if (IsShortString())
			{
				return static_cast<SizeType>(sizeof(Short.Data) - (Short.InvertedSize));
			}

			return Heap.Size;
		}

		constexpr StringSpan Substring(SizeType start)
		{
			return StringSpan{ Data() + start, end() };
		}

		constexpr StringView Substring(SizeType start) const
		{
			return StringView{ Data() + start, end() };
		}

		constexpr StringSpan Substring(SizeType start, SizeType count)
		{
			return StringSpan{ Data() + start, count };
		}

		constexpr StringView Substring(SizeType start, SizeType count) const
		{
			return StringView{ Data() + start, count };
		}

		constexpr StringImplementation& TrimBegin()
		{
			auto codepointIt = CodepointBegin();

			while (codepointIt != CodepointEnd() && codepointIt->IsWhitespace())
			{
				removeCodepointIterator(codepointIt);
			}

			return *this;
		}

		static StringImplementation TrimBegin(const StringImplementation& value)
		{
			auto result = value;
			result.TrimBegin();
			return result;
		}

		constexpr StringImplementation& TrimEnd()
		{
			if (!IsEmpty())
			{
				auto codepointIt = CodepointEnd() - 1;
				auto codepointItEnd = CodepointBegin() - 1;

				while (codepointIt != codepointItEnd && codepointIt->IsWhitespace())
				{
					removeCodepointIterator(codepointIt);
					--codepointIt;
				}
			}

			return *this;
		}

		static StringImplementation TrimEnd(const StringImplementation& value)
		{
			auto result = value;
			result.TrimEnd();
			return result;
		}

		constexpr StringImplementation& Trim()
		{
			TrimBegin();
			TrimEnd();

			return *this;
		}

		static StringImplementation Trim(const StringImplementation& value)
		{
			auto result = value;
			result.Trim();
			return result;
		}

		constexpr StringSpan ToStringSpan()
		{
			return StringSpan{ begin(), end() };
		}

		constexpr StringView ToStringView() const
		{
			return StringView{ begin(), end() };
		}

		constexpr Pointer begin()
		{
			return Data();
		}

		constexpr ConstPointer begin() const
		{
			return Data();
		}

		constexpr Pointer end()
		{
			return Data() + Size();
		}

		constexpr ConstPointer end() const
		{
			return Data() + Size();
		}

		constexpr Reference operator[](SizeType index)
		{
			return Data()[index];
		}

		constexpr ConstReference operator[](SizeType index) const
		{
			return Data()[index];
		}

		constexpr StringImplementation& operator+=(Value value)
		{
			Append(value);
			return *this;
		}

		constexpr StringImplementation& operator+=(StringView view)
		{
			Append(view);
			return *this;
		}

		constexpr StringImplementation& operator+=(Unicode::Codepoint codepoint)
		{
			Append(codepoint);
			return *this;
		}

		constexpr operator StringView() const
		{
			return ToStringView();
		}

	private:
		Allocator& allocator()
		{
			return *this;
		}

		Pointer allocate(SizeType size)
		{
			return static_cast<Pointer>(BAROQUE_ALLOC(allocator(), size, String));
		}

		void assign(ConstPointer value, SizeType size)
		{
			if (size > (IsUsingStackCapacity ? (Allocator::StackCapacity - 1) : sizeof(Short.Data)))
			{
				auto newCapacity = size;
				setCapacity(newCapacity);
				Heap.Data = allocate(newCapacity + 1);

				std::memset(Heap.Data, 0, newCapacity + 1);
			}

			std::memcpy(Data(), value, size);
			setSize(size);
		}

		constexpr void ensureCapacity()
		{
			auto capacity = Capacity();

			if (Size() == capacity)
			{
				Reserve(static_cast<SizeType>(static_cast<double>(capacity) * GrowFactor));
			}
		}

		constexpr void ensureCapacity(SizeType requiredCapacity)
		{
			auto currentCapacity = Capacity();

			if (currentCapacity < requiredCapacity)
			{
				auto newCapacity = Algorithm::Max(requiredCapacity, static_cast<SizeType>(static_cast<double>(currentCapacity) * GrowFactor));
				Reserve(newCapacity);
			}
		}

		constexpr bool isHeap() const
		{
			return Short.InvertedSize & ShortMask;
		}

		void internalDestructor()
		{
			if (isHeap() && Heap.Data)
			{
				allocator().Deallocate(Heap.Data);
			}
		}

		void insertCodepoint(Pointer data, Unicode::Codepoint codepoint, SizeType codepointLength)
		{
			switch (codepointLength)
			{
				case 1:
				{
					data[0] = static_cast<char>(codepoint.Value);
					break;
				}
				case 2:
				{
					data[0] = static_cast<char>(Unicode::TwoByteMask | ((codepoint.Value >> 6) & 0x1F));
					data[1] = static_cast<char>(Unicode::SequenceByteMask | (codepoint.Value & 0x3F));
					break;
				}
				case 3:
				{
					data[0] = static_cast<char>(Unicode::ThreeByteMask | ((codepoint.Value >> 12) & 0x0F));
					data[1] = static_cast<char>(Unicode::SequenceByteMask | ((codepoint.Value >> 6) & 0x3F));
					data[2] = static_cast<char>(Unicode::SequenceByteMask | (codepoint.Value & 0x3F));
					break;
				}
				case 4:
				{
					data[0] = static_cast<char>(Unicode::FourByteMask | ((codepoint.Value >> 18) & 0x07));
					data[1] = static_cast<char>(Unicode::SequenceByteMask | ((codepoint.Value >> 12) & 0x3F));
					data[2] = static_cast<char>(Unicode::SequenceByteMask | ((codepoint.Value >> 6) & 0x3F));
					data[3] = static_cast<char>(Unicode::SequenceByteMask | (codepoint.Value & 0x3F));
					break;
				}
				default:
					break;
			}
		}

		void moveLeft(Pointer data, SizeType start, SizeType end, SizeType step)
		{
			auto* sourceIt = data + start + step;
			auto* destinationIt = data + start;
			auto* sourceItEnd = data + end;

			for (; sourceIt != sourceItEnd; ++sourceIt, ++destinationIt)
			{
				*destinationIt = *sourceIt;
			}
		}

		void moveRight(Pointer data, SizeType start, SizeType end, SizeType step)
		{
			auto* sourceIt = data + (end - 1);
			auto* destinationIt = data + (end) + (step - 1);

			auto* sourceItEnd = data + (start - 1);

			for (; sourceIt != sourceItEnd; --sourceIt, --destinationIt)
			{
				*destinationIt = *sourceIt;
			}
		}

		constexpr void reallocate(SizeType newCapacity)
		{
			auto* oldData = Data();

			auto* newData = allocate(newCapacity + 1);
			std::memset(newData, 0, newCapacity + 1);

			if (oldData)
			{
				std::memcpy(newData, oldData, Size());

				if (oldData != Short.Data)
				{
					allocator().Deallocate(oldData);
				}
			}

			setCapacity(newCapacity);

			Heap.Data = newData;
		}

		constexpr void removeCodepointIterator(Unicode::CodepointIterator it)
		{
			auto data = Data();
			auto size = Size();
			auto codepointLength = it->Length();

			moveLeft(data, it.GetRawIterator() - data, size, codepointLength);

			auto newSize = size - codepointLength;

			data[newSize] = '\0';

			setSize(newSize);
		}

		constexpr void setSize(SizeType size)
		{
			if (isHeap())
			{
				Heap.Size = size;
			}
			else
			{
				Short.InvertedSize = static_cast<std::uint8_t>(sizeof(Short.Data) - size);
			}
		}

		constexpr void setCapacity(SizeType size)
		{
			Heap.Capacity = HeapMask | size;
		}

		void zeroInit()
		{
			std::memset(&Short, 0, sizeof(Short));
		}

	private:
		struct HeapType
		{
			Pointer Data;
			SizeType Size;
			SizeType Capacity;
		};

		struct ShortType
		{
			Value Data[sizeof(HeapType) - 1];
			std::uint8_t InvertedSize;
		};

		union
		{
			ShortType Short;
			HeapType Heap;
		};
	};

	template<typename Allocator>
	inline constexpr StringImplementation<Allocator> operator+(const StringImplementation<Allocator>& left, const StringImplementation<Allocator>& right)
	{
		auto leftSize = left.Size();
		auto rightSize = right.Size();

		StringImplementation<Allocator> result(leftSize + rightSize);

		std::memcpy(result.Data(), left.Data(), leftSize);
		std::memcpy(result.Data() + leftSize, right.Data(), rightSize);

		return result;
	}

	template<typename Allocator>
	inline constexpr StringImplementation<Allocator> operator+(const StringImplementation<Allocator>& left, StringView right)
	{
		auto leftSize = left.Size();
		auto rightSize = right.Size();

		StringImplementation<Allocator> result(leftSize + rightSize);

		std::memcpy(result.Data(), left.Data(), leftSize);
		std::memcpy(result.Data() + leftSize, right.Data(), rightSize);

		return result;
	}

	template<typename Allocator>
	inline constexpr StringImplementation<Allocator> operator+(StringView left, const StringImplementation<Allocator>& right)
	{
		auto leftSize = left.Size();
		auto rightSize = right.Size();

		StringImplementation<Allocator> result(leftSize + rightSize);

		std::memcpy(result.Data(), left.Data(), leftSize);
		std::memcpy(result.Data() + leftSize, right.Data(), rightSize);

		return result;
	}

	using String = StringImplementation<Baroque::Memory::DefaultAllocator>;

	static_assert(sizeof(String) == 24, "Baroque::String is not 24 bytes");

	template<std::size_t Size>
	using SmallString = StringImplementation<Baroque::Memory::SmallAllocator<Size>>;

	namespace Literals
	{
		inline String operator""_s(const char* value, std::size_t size)
		{
			return { value, size };
		}
	}
}