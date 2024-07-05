#pragma once
#include "FoxtrotEngine/Actors/Transform.h"

#include "FoxtrotEngine/Renderer/Rect.h"

class RectTransform :
    public Transform
{
public:
    const FTVector2   GetAnchoredPosition() const { return mAnchoredPosition; }
    const FTVector2   GetPivot()  const { return mPivot; }
    const Rect        GetRect()   const { return mRect; }

    void SetAnchoredPosition(FTVector2 pos) { mAnchoredPosition = pos; }
    void SetPivot(FTVector2 point) { mPivot = point; }
    void SetRect(Rect rect) { mRect = rect; }

private:
    FTVector2 mAnchoredPosition;
    FTVector2 mPivot;
    Rect      mRect;

public:
    RectTransform();
};

