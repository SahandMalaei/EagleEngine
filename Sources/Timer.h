#ifndef _TIMER_H
#define _TIMER_H 1

#include <time.h>

#include <Windows.h>

namespace ProjectEagle
{
	class Timer
	{
	private:
		int timerStart;
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

#endif