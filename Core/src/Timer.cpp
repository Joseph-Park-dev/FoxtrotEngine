// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Timer.h"

namespace Core
{
	Timer::Timer()
		: mDeltaTime(0.f)
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&mFrequencyForASecond);
		QueryPerformanceCounter((LARGE_INTEGER*)&mLastTicks);

		mTimeScale = 1.0f / (float)mFrequencyForASecond;
	}

	void Timer::Update()
	{
		LARGE_INTEGER currentTicks;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTicks);
		while (!FT_TICKS_PASSED(currentTicks.QuadPart, mLastTicks.QuadPart + 16))
			;
		mDeltaTime = (float)(currentTicks.QuadPart - mLastTicks.QuadPart) * mTimeScale;
		if (mDeltaTime > 0.05f)
		{
			mDeltaTime = 0.05f;
		}
		mLastTicks = currentTicks;
	}

	Timer::~Timer() {}
} // namespace Core