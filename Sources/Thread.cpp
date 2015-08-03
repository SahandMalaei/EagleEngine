#include "Thread.h"

namespace ProjectEagle
{
	/*static DWORD WINAPI ThreadProc(Thread *thread )
	{
		return thread->threadFunction();
	}

	Thread::Thread()
	{
		isRunning = false;
	}

	void Thread::Initialize()
	{
		threadHandle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadProc, this, 0, (LPDWORD)&ID);
		if(threadHandle == 0) Eagle->error("Thread creation failed.");
		isRunning = true;
	}

	void Thread::End()
	{
		if(threadHandle)
		{
			isRunning = false;
			WaitForSingleObject(threadHandle, INFINITE);
			CloseHandle(threadHandle);
			threadHandle = 0;
		}
	}

	ThreadMonitor::ThreadMonitor()
	{
		mutexHandle = CreateMutex(0, false, 0);
		if(!mutexHandle) eagle->error("Mutex creation failed.");
	}

	ThreadMonitor::~ThreadMonitor()
	{
		if(mutexHandle)
		{
			CloseHandle(mutexHandle);
			mutexHandle = 0;
		}
	}

	void ThreadMonitor::MutexOn() const
	{
		WaitForSingleObject(mutexHandle, INFINITE);
	}

	void ThreadMonitor::MutexOff() const
	{
		ReleaseMutex(mutexHandle);
	}*/
};