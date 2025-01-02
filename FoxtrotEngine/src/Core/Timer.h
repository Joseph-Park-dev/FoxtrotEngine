// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// This ensures the game runs in the consistent context of time.
/// Responsible to update the Delta Time.
/// </summary>

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