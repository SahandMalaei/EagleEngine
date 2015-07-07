#pragma once
#include <time.h>
#include <Eagle.h>
//#include <Windows.h>
namespace ProjectEagle
{
	class Timer
	{
	private:
		DWORD timerStart;
		LARGE_INTEGER timeCounter, frequency;

	public:
		Timer(void);
		~Timer(void);
		float getTimeSeconds();
		float getPassedTimeSeconds();
		float getTimeMilliseconds();
		float getPassedTimeMilliseconds();
		void reset();
		bool stopWatch(float seconds);
	};
};