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
	struct AutoTryReadLock
	{
		AutoTryReadLock(Lock& lock)
		: _lock(lock)
		{
			_needUnlock = lock.TryLockRead();
		}

		~AutoTryReadLock()
		{
			if (_needUnlock)
			{
				_lock.Unlock();
			}
		}

	private:
		bool _needUnlock = false;
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

	template<typename Lock>
	struct AutoTryWriteLock
	{
		AutoTryWriteLock(Lock& lock)
		: _lock(lock)
		{
			_needUnlock = lock.TryLockWrite();
		}

		~AutoTryWriteLock()
		{
			if (_needUnlock)
			{
				_lock.Unlock();
			}
		}

	private:
		bool _needUnlock = false;
		Lock& _lock;
	};
}