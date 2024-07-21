#include "Scripts/ActorList/MouseCursor.h"

#include "FoxtrotEngine/ResourceSystem/Tile.h"
#include "FoxtrotEngine/Components/TileMapComponent.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/Managers/KeyInputManager.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"

void MouseCursor::UpdateActor(float deltaTime)
{
	mCursorPosition = MOUSE_POS;
}

void MouseCursor::RenderActor(FoxtrotRenderer* renderer)
{
	if (mTM != nullptr)
	{
		Tile* cursorIMG = mTM->GetTile(0, 0);
		//mTM->DrawIndividualTileOnPos(renderer, mCursorPosition, cursorIMG);
	}
}

MouseCursor::MouseCursor(Scene* scene)
	: UIActor(scene, false)
	, mTM(new TileMapComponent(this, DEFAULT_DRAWORDER))
	, mCursorPosition(FTVector2::Zero)
	, mOffsetSelect(FTVector2(23, 17))
{
	//SDL_ShowCursor(SDL_DISABLE);
	//mTM->InitializeTileMap(L"Assets/TileMapCSVs/MouseCursors.csv", 80, 80);
	//FTTexture* tileMap = ResourceManager::GetInstance()->GetLoadedTexture(L"MouseCursor");
	//mTM->SetTexture(tileMap);
	//mOffsetAim = FTVector2(mTM->GetTexWidth() / 2, mTM->GetTexHeight() / 2);
}