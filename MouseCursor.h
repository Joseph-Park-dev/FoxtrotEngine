#pragma once
#include "UIActor.h"

class FTTexture;
class TileMapComponent;

class MouseCursor :
    public UIActor
{
public:
    TileMapComponent* GetTileMapComp() { return mTM; }
    FTVector2         GetCursorPosition() const { return mCursorPosition; }

public:
    virtual void UpdateActor(float deltaTime) override;
    virtual void RenderActor(FoxtrotRenderer* renderer) override;

public:
    MouseCursor(Scene* scene);

private:
    TileMapComponent* mTM;
    FTVector2         mCursorPosition;
    FTVector2         mOffsetSelect;
    FTVector2         mOffsetAim;
};

