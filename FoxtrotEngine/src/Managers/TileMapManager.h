#pragma once
#include "Core/SingletonMacro.h"

#include <Windows.h>

class FTTileMap;

class TileMapManager
{
	SINGLETON(TileMapManager)

private:
	void SaveTileMapAsFile(FTTileMap* tileMap, UINT key);

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI(bool* opened);
#endif
};

