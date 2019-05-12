#include <gtest/gtest.h>

#include "Core/Threading/ReaderWriterLock.h"

// TODO: Use my own thread class
#include <thread>

using namespace std::chrono_literals;

namespace
{
	void lockReadThreadFunction(Baroque::ReaderWriterLock* mutex)
	{
		mutex->LockRead();
		std::this_thread::sleep_for(100ms);
		mutex->UnlockRead();
	}

	void lockWriteThreadFunction(Baroque::ReaderWriterLock* mutex)
	{
		mutex->LockWrite();
		std::this_thread::sleep_for(100ms);
		mutex->UnlockWrite();
	}
}

TEST(ReaderWriterLock, ShouldBeAbleToLockReadMultipleTimes)
{
	Baroque::ReaderWriterLock lock;
	std::thread firstThread(lockReadThreadFunction, &lock);
	std::thread secondThread(lockReadThreadFunction, &lock);
	EXPECT_TRUE(lock.TryLockRead());
	firstThread.join();
	secondThread.join();
	lock.UnlockRead();
}

TEST(ReaderWriterLock, LockWriteShouldBeExclusive)
{
	Baroque::ReaderWriterLock lock;
	std::thread testThread(lockWriteThreadFunction, &lock);
	std::this_thread::sleep_for(20ms);
	EXPECT_FALSE(lock.TryLockRead());
	EXPECT_FALSE(lock.TryLockWrite());
	testThread.join();

	EXPECT_TRUE(lock.TryLockWrite());
	lock.UnlockWrite();
}