#include "Eagle.h"

namespace ProjectEagle
{
	Timer::Timer(void)
	{
		//timerStart = eagle.getTimer()->getPassedTimeMilliseconds();
		QueryPerformanceCounter(&timeCounter);
		QueryPerformanceFrequency(&frequency);
		timerStart = timeCounter.LowPart;
		reset();
	}

	Timer::~Timer(void)
	{
	}

	float Timer::getTimeSeconds()
	{
		QueryPerformanceCounter(&timeCounter);
		
		return ((float)timeCounter.LowPart / (float)frequency.LowPart);
	}

	float Timer::getPassedTimeSeconds()
	{
		QueryPerformanceCounter(&timeCounter);
		
		return ((float)(timeCounter.LowPart - timerStart) / (float)frequency.LowPart);
	}

	float Timer::getTimeMilliseconds()
	{
		return (float)getTimeSeconds() * 1000.0;
	}

	float Timer::getPassedTimeMilliseconds()
	{
		return (float)getPassedTimeSeconds() * 1000.0;
	}

	void Timer::reset()
	{
		LARGE_INTEGER timeCounter;
		QueryPerformanceCounter(&timeCounter);

		timerStart = timeCounter.LowPart;
	}

	bool Timer::stopWatch(float seconds)
	{
		if(getPassedTimeSeconds() >= seconds)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
};