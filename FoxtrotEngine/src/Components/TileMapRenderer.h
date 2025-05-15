// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// This renders FTTilemaps using FTTexture as a texture pack,
/// primitive square as a mesh, and FTTileMap as a .CSV source.
/// </summary>

#pragma once
#include "Components/SpriteRenderer.h"

#include <string>

#include "Core/TemplateFunctions.h"

class FTVector2;
class FTTileMap;

class TileMapRenderer :
	public SpriteRenderer
{
public:
	std::string GetName() const override
	{
		return "TileMap";
	}
	const char* GetTileMapKey() const;
	FTTileMap*	GetTileMap() const;

	void SetTileMapKey(const char* key);
	void SetTileMap(FTTileMap* tileMap);

public:
	virtual void Initialize(FTCore* coreInstance) override;
	virtual void CloneTo(Actor* actor) override;

public:
	TileMapRenderer(
		Actor* owner,
		int	   UpdateOrder = DefaultVal::UPDATE_ORDER);
	virtual ~TileMapRenderer() override;

protected:
	virtual void InitializeTileMap();

private:
	FTTileMap*	mTileMap;
	const char* mTileMapKey;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void EditorUIUpdate() override;
	virtual void OnConfirmUpdate() override;

protected:
	void UpdateCSV();
	void UpdateCSV(const char* key);
#endif
};

namespace ChunkKey
{
	constexpr const char* TILEMAP_KEY = "TileMapKey";
}