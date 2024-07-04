#pragma once
#include "FoxtrotEngine/Actors/Transform.h"

class RectTransform :
    public Transform
{
private:
    FTVector2 mAnchoredPosition;
    FTVector2 mPivot;

};

