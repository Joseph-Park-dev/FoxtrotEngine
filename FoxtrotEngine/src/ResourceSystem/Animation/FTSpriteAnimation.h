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
struct AnimationFrame;

class FTSpriteAnimation : public FTAnimation
{
public:
	UINT			GetTileDataKey();
	AnimationFrame* GetFrame(int frameIdx);

	void SetTileDataKey(UINT key);

public:
	FTSpriteAnimation();
	FTSpriteAnimation(FTSpriteAnimation* other);
	~FTSpriteAnimation() override;

private:
	// Key to the FTResource that contains Tile data 
	// (FTTileMap, FTSpriteSheet)
	UINT mTileDataKey;

public:
	virtual void SaveProperties(std::ofstream& ofs, UINT key) override;
	virtual UINT LoadProperties(std::ifstream& ifs) override;
};

namespace ChunkKey
{
	namespace SpriteAnimation
	{
		constexpr const char* FT_SPRITE_ANIMATION = "FTSpriteAnimation";
		constexpr const char* ANIM_TILEMAP_KEY	  = "TileMap Key";
	} // namespace SpriteAnimation
} // namespace ChunkKey