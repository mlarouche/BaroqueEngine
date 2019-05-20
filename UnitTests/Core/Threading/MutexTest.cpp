#include <gtest/gtest.h>

#include "Core/Threading/Mutex.h"

// TODO: Use my own thread class
#include <thread>

using namespace std::chrono_literals;

namespace
{
	void mutexTestThreadFunction(Baroque::Mutex* mutex)
	{
		mutex->Lock();
		std::this_thread::sleep_for(100ms);
		mutex->Unlock();
	}
}

TEST(Mutex, ShouldLock)
{
	Baroque::Mutex lock;
	std::thread testThread(mutexTestThreadFunction, &lock);
	std::this_thread::sleep_for(20ms);
	EXPECT_FALSE(lock.TryLock());
	testThread.join();
}
