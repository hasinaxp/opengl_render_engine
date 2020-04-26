#pragma once
#include "../api.h"

namespace sp {

	//time is calculated as seconds
	class SP_API Timer {
	private:
		float _totalTime;
		uint _lastTimeRecord;
	public:
		Timer(float totalTime);
		void reset();
		void reset(float totalTime);
		float fraction();
	};


}
