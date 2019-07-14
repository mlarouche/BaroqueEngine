#include "StringSpan.h"

#include "Core/Containers/StringView.h"
#include "Core/Containers/String.h"

namespace Baroque
{
	bool StringSpan::Contains(StringView view) const
	{
		return Find(view) != nullptr;
	}

	bool StringSpan::EndsWith(StringView view) const
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

	StringSpan::Pointer StringSpan::Find(StringView view)
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

	StringSpan::ConstPointer StringSpan::Find(StringView view) const
	{
		return const_cast<ConstPointer>(const_cast<StringSpan*>(this)->Find(view));
	}

	StringSpan::SizeType StringSpan::IndexOf(StringView view) const
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

	StringSpan::SizeType StringSpan::LastIndexOf(StringView view) const
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

	StringSpan::Pointer StringSpan::ReverseFind(StringView view)
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

	StringSpan::ConstPointer StringSpan::ReverseFind(StringView view) const
	{
		return const_cast<ConstPointer>(const_cast<StringSpan*>(this)->ReverseFind(view));
	}

	StringView StringSpan::Slice(SizeType start, SizeType end) const
	{
		return StringView{ Data() + start, Data() + ((end - start) + 1) };
	}

	bool StringSpan::StartsWith(StringView view) const
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

	StringView StringSpan::Substring(SizeType start) const
	{
		return StringView{ Data() + start, end() };
	}

	StringView StringSpan::Substring(SizeType start, SizeType count) const
	{
		return StringView{ Data() + start, count };
	}

	StringImplementation<Baroque::Memory::DefaultAllocator> StringSpan::ToString() const
	{
		return { Data(), Size() };
	}
}
