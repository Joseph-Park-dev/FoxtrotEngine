#include "MouseCursor.h"
#include "Tile.h"
#include "TileMapComponent.h"
#include "ResourceManager.h"
#include "KeyInputManager.h"

void MouseCursor::UpdateActor(float deltaTime)
{
	mCursorPosition = MOUSE_POS;
}

void MouseCursor::RenderActor(SDL_Renderer* renderer)
{
	if (mTM != nullptr)
	{
		Tile* cursorIMG = mTM->GetTile(0, 0);
		mTM->DrawIndividualTileOnPos(renderer, mCursorPosition, cursorIMG);
	}
}

MouseCursor::MouseCursor(Scene* scene)
	: UIActor(scene, false)
	, mTM(new TileMapComponent(this, DEFAULT_DRAWORDER))
	, mCursorPosition(Vector2::Zero)
	, mOffsetSelect(Vector2(23, 17))
{
	SDL_ShowCursor(SDL_DISABLE);
	mTM->InitializeTileMap(L"Assets/TileMapCSVs/MouseCursors.csv", 80, 80);
	Texture* tileMap = ResourceManager::GetInstance()->GetLoadedTexture(L"MouseCursor");
	mTM->SetTexture(tileMap);
	//mOffsetAim = Vector2(mTM->GetTexWidth() / 2, mTM->GetTexHeight() / 2);
}