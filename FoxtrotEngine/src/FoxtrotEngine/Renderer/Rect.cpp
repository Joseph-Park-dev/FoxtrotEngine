#include "Rect.h"

bool Rect::Contains(const FTVector2& point){
    return mMin.x <= point.x && point.x <= mMax.x
        && mMin.y <= point.y && point.y <= mMax.y;
}

bool Rect::Overlaps(const Rect& other)
{
    FTVector2 rightMin = other.mMin;
    FTVector2 rightMax = other.mMax;
    return Contains(other.mMin) || Contains(other.mMax);
}
