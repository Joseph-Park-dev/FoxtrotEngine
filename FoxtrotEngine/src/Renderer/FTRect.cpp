// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer/FTRect.h"

const FTRect FTRect::Zero(0.0f, 0.0f, 0.0f, 0.0f);

bool FTRect::Contains(const FTVector2& point){
    return mMin.x <= point.x && point.x <= mMax.x
        && mMin.y <= point.y && point.y <= mMax.y;
}

bool FTRect::Overlaps(const FTRect& other)
{
    FTVector2 rightMin = other.mMin;
    FTVector2 rightMax = other.mMax;
    return Contains(other.mMin) || Contains(other.mMax);
}
