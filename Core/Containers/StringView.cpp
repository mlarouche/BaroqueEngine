#include "StringView.h"

#include "Core/Containers/String.h"

namespace Baroque
{
	StringImplementation<Baroque::Memory::DefaultAllocator> StringView::ToString() const
	{
		return { Data(), Size() };
	}
}