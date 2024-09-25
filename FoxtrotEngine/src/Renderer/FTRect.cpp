#include "Renderer/FTRect.h"

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
