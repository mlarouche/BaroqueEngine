#pragma once

#include "Core/CoreDefines.h"

#include "Core/Platforms/Win32/MinimalWindowsIncludes.h"
#include <synchapi.h>

namespace Baroque
{
	class ReaderWriterLock
	{
	public:
		ReaderWriterLock()
		{
			InitializeSRWLock(&_lock);
		}

		void LockRead()
		{
			AcquireSRWLockShared(&_lock);
		}

		void UnlockRead()
		{
			ReleaseSRWLockShared(&_lock);
		}

		bool TryLockRead()
		{
			return TryAcquireSRWLockShared(&_lock) > 0;
		}

		void LockWrite()
		{
			AcquireSRWLockExclusive(&_lock);
		}

		void UnlockWrite()
		{
			ReleaseSRWLockExclusive(&_lock);
		}

		bool TryLockWrite()
		{
			return TryAcquireSRWLockExclusive(&_lock);
		}

	private:
		::SRWLOCK _lock;
	};
}