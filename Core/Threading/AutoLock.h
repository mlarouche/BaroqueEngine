#pragma once

#include "Core/CoreDefines.h"

namespace Baroque
{
	template<typename Lock>
	struct AutoLock
	{
		AutoLock(Lock& lock)
		: _lock(lock)
		{
			_lock.Lock();
		}

		~AutoLock()
		{
			_lock.Unlock();
		}

	private:
		Lock& _lock;
	};

	template<typename Lock>
	struct AutoReadLock
	{
		AutoReadLock(Lock& lock)
		: _lock(lock)
		{
			_lock.LockRead();
		}

		~AutoReadLock()
		{
			_lock.UnlockRead();
		}

	private:
		Lock& _lock;
	};

	template<typename Lock>
	struct AutoWriteLock
	{
		AutoWriteLock(Lock& lock)
		: _lock(lock)
		{
			_lock.LockWrite();
		}

		~AutoWriteLock()
		{
			_lock.UnlockWrite();
		}

	private:
		Lock& _lock;
	};
}