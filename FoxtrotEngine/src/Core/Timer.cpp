// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Core/Timer.h"

Timer::Timer()
{
    QueryPerformanceFrequency((LARGE_INTEGER*)&mFrequencyForASecond);
    //1초동안 CPU의 진동수를 반환하는 함수 (고정값. 시스템 부팅시 고정됨)

    QueryPerformanceCounter((LARGE_INTEGER*)&mLastTicks);
    // 생성자함수는 Update()함수보다 이전에 실행되기 때문에 lastTime에 저장.

    mTimeScale = 1.0 / (float)mFrequencyForASecond;
    // 미리  '1 / 초당진동수'  의 형태로 만들어 놓는다.
    // 컴퓨터는 나눗셈연산보다 곱셈연산이 더 빠르기때문에 매 프레임마다 연산을 해야할경우 
    // 역수로 만들어놓고 실제 계산때 곱셈하게 만들어놓음
}

void Timer::Update()
{
    LARGE_INTEGER currentTicks;
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTicks); // 현재 프레임 진동수 저장.
    while (!FT_TICKS_PASSED(currentTicks.QuadPart, mLastTicks.QuadPart + 16));
    mDeltaTime = (float)(currentTicks.QuadPart - mLastTicks.QuadPart) * mTimeScale; // (현재 프레임 진동수 - 이전 프레임 진동수) * ( 1 / 초당진동수 )
    if (mDeltaTime > 0.05f)
    {
        mDeltaTime = 0.05f;
    }
    mLastTicks = currentTicks; // 현재 프레임을 이전 프레임으로 저장. 이후 계속 반복.
}

Timer::~Timer(){}