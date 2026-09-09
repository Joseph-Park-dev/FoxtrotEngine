// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Manager/Timer.h"

namespace Core
{
	/// @brief Initializes the high-resolution frame timing state.
	/// @note Initializes the :Timer base or delegates to its constructor.
	Timer::Timer()
		: mDeltaTime(0.f)
	{
		QueryPerformanceFrequency((LARGE_INTEGER*)&mFrequencyForASecond);
		QueryPerformanceCounter((LARGE_INTEGER*)&mLastTicks);

		mTimeScale = 1.0f / (float)mFrequencyForASecond;
	}

	/// @brief Advances frame-dependent state using the current time step.
	void Timer::Update()
	{
		LARGE_INTEGER currentTicks;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTicks);
		mDeltaTime = (float)(currentTicks.QuadPart - mLastTicks.QuadPart) * mTimeScale;
		if (mDeltaTime > 0.05f)
		{
			mDeltaTime = 0.05f;
		}
		mLastTicks = currentTicks;
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	Timer::~Timer() {}
} // namespace Core
