#include "utils.h"
#include <SDL.h>

namespace sp {


	Timer::Timer(float totalTime)
		:_totalTime(totalTime)
	{
		_lastTimeRecord = SDL_GetTicks();
	}

	void Timer::reset()
	{
		_lastTimeRecord = SDL_GetTicks();
	}

	void Timer::reset(float totalTime)
	{
		_totalTime = totalTime;
		_lastTimeRecord = SDL_GetTicks();
	}

	float Timer::fraction()
	{
		uint nt = SDL_GetTicks();
		float frac = float(nt - _lastTimeRecord) / (_totalTime * 1000);
		if (frac >= 1.0) {
			reset();
			frac = fraction();
		}
		return frac;
	}




};

