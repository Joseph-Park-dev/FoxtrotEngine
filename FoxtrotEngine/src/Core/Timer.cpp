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
    //1�ʵ��� CPU�� �������� ��ȯ�ϴ� �Լ� (������. �ý��� ���ý� ������)

    QueryPerformanceCounter((LARGE_INTEGER*)&mLastTicks);
    // �������Լ��� Update()�Լ����� ������ ����Ǳ� ������ lastTime�� ����.

    mTimeScale = 1.0 / (float)mFrequencyForASecond;
    // �̸�  '1 / �ʴ�������'  �� ���·� ����� ���´�.
    // ��ǻ�ʹ� ���������꺸�� ���������� �� �����⶧���� �� �����Ӹ��� ������ �ؾ��Ұ�� 
    // ������ �������� ���� ��궧 �����ϰ� ��������
}

void Timer::Update()
{
    LARGE_INTEGER currentTicks;
    QueryPerformanceCounter((LARGE_INTEGER*)&currentTicks); // ���� ������ ������ ����.
    while (!FT_TICKS_PASSED(currentTicks.QuadPart, mLastTicks.QuadPart + 16));
    mDeltaTime = (float)(currentTicks.QuadPart - mLastTicks.QuadPart) * mTimeScale; // (���� ������ ������ - ���� ������ ������) * ( 1 / �ʴ������� )
    if (mDeltaTime > 0.05f)
    {
        mDeltaTime = 0.05f;
    }
    mLastTicks = currentTicks; // ���� �������� ���� ���������� ����. ���� ��� �ݺ�.
}

Timer::~Timer(){}