#pragma once

#include "Core/CoreDefines.h"

#include "Core/Platforms/Win32/MinimalWindowsIncludes.h"
#include <synchapi.h>

namespace Baroque
{
	class Mutex
	{
	public:
		Mutex()
		{
			InitializeCriticalSection(&_criticalSection);
		}
		~Mutex()
		{
			DeleteCriticalSection(&_criticalSection);
		}

		void Lock()
		{
			EnterCriticalSection(&_criticalSection);
		}

		void Unlock()
		{
			LeaveCriticalSection(&_criticalSection);
		}

		bool TryLock()
		{
			return TryEnterCriticalSection(&_criticalSection) > 0;
		}

	private:
		::CRITICAL_SECTION _criticalSection;
	};
}