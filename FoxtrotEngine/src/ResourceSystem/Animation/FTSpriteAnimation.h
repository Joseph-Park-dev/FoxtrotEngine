// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A class that holds the Sprite Animation.
/// This will be registered to the Animator Component assigned to
/// an Actor.
/// </summary>

#pragma once
#include "ResourceSystem/Animation/FTAnimation.h"

#include <vector>
#include <string>

#include "Core/TemplateFunctions.h"
#include "Math/FTMath.h"
#include "ResourceSystem/Tile.h"

class Tile;
class Animator;
class FTTexture;

class FTSpriteAnimation : public FTAnimation
{
public:
	UINT GetTileMapKey();

	void SetTileMapKey(UINT key);

public:
	FTSpriteAnimation();
	FTSpriteAnimation(FTSpriteAnimation* other);
	~FTSpriteAnimation() override;

private:
	// These fields need to be loaded from .chunk file
	UINT mTileMapKey;

public:
	virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
	virtual UINT LoadProperties(std::ifstream& ifs) override;
};

namespace ChunkKey
{
	constexpr const char* FT_SPRITE_ANIMATION  = "FTSpriteAnimation";
	constexpr const char* ANIM_NAME			   = "Name";
	constexpr const char* ANIM_FPS			   = "FPS";
	constexpr const char* ANIM_IS_REPEATED	   = "Is Repeated";
	constexpr const char* ANIM_MAX_FRAME_INDEX = "Max Frame Index";
	constexpr const char* ANIM_TILEMAP_KEY	   = "Tilemap Key";
} // namespace ChunkKey