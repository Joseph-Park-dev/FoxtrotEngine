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
	AnimationFrame* GetFrame(int frameIdx);

public:
	FTSpriteAnimation();
	FTSpriteAnimation(FTSpriteAnimation* other);
	~FTSpriteAnimation() override;

private:
	FTSpriteSheet* mSpriteSheet;

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;
	virtual void Process(FTCore* coreInst) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void AddRefCount() override;
	virtual void SubtractRefCount() override;
#endif
};

namespace ChunkKey
{
	namespace SpriteAnimation
	{
		constexpr const char* FT_SPRITE_ANIMATION = "FTSpriteAnimation";
		constexpr const char* ANIM_TILEMAP_KEY	  = "TileMap Key";
	} // namespace SpriteAnimation
} // namespace ChunkKey