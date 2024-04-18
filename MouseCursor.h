#pragma once
#include "UIActor.h"

class FTTexture;
class TileMapComponent;

class MouseCursor :
    public UIActor
{
public:
    TileMapComponent* GetTileMapComp() { return mTM; }
    Vector2           GetCursorPosition() const { return mCursorPosition; }

public:
    virtual void UpdateActor(float deltaTime) override;
    virtual void RenderActor(FoxtrotRenderer* renderer) override;

public:
    MouseCursor(Scene* scene);

private:
    TileMapComponent* mTM;
    Vector2 mCursorPosition;
    Vector2 mOffsetSelect;
    Vector2 mOffsetAim;
};

