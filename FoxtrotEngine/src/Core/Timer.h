#pragma once
#include <profileapi.h>

#include "Core/SingletonMacro.h"

#define FT_TICKS_PASSED(A, B)  ((__int64)((B) - (A)) <= 0)

class Timer
{
	SINGLETON(Timer);
public:
	void   Update();
	float  GetDeltaTime() const { return mDeltaTime; }

private:
	__int64			mFrequencyForASecond;
	LARGE_INTEGER	mLastTicks;
	float			mTimeScale;
	
	float			mDeltaTime;
};