// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <ResourceSystem/FTMeshGroup.h>

#include <vector>
#include <string>

#include "Core/TemplateFunctions.h"
#include "Math/FTMath.h"
#include "ResourceSystem/Tile.h"

class Tile;
class Animator;
class FTTexture;
struct AnimationFrame;

/// @brief A FTResource that holds a Sprite Animation.
/// This will be registered to a SpriteAnimator Component instance.
class FTSpriteAnimation : public FTMeshGroup
{
public:
	/// @brief Takes tiles array and generate sprite animation base on its data.
	/// Creates one Mesh object per every sprite frame, adding it to the MeshGroup.
	/// @param tiles Tile data consists of Rect area on sprite sheet & game screen.
	void Initialize(
		const Tile*					 tiles,
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context);

	/// @see FTResource::SaveProperties()
	virtual void SaveProperties(std::ofstream& ofs) override;

	/// @see FTResource::LoadProperties()
	virtual void LoadProperties(std::ifstream& ifs) override;

public:
	/// @brief Returns Frames per second
	const int GetFPS() const;

	/// @brief Returns the last frame index.
	const int GetMaxFrameIdx() const;

	/// @brief Returns the first frame index.
	const int GetMinFrameIdx() const;

public:
	/// @brief Relative path is used for importing .spriteanim file.
	FTSpriteAnimation(FTResourceDef& resDef, FoxtrotRenderer* renderer);
	~FTSpriteAnimation();

protected:
	/// @brief Takes an array of Tiles, initializes a sprite animation.
	virtual void Process(FoxtrotRenderer* renderer) override;

private:
	/// @brief A text file that holds the rect data on a spritesheet.
	FTText* mAtlas;

	/// @brief Frames-per-second for this animation.
	int mFPS;

	/// @brief Should this animation be looped?
	bool mIsRepeated;

	/// @brief the first frame index.
	int mMinFrameIdx;

	/// @brief the last frame index.
	int mMaxFrameIdx;

#ifdef FOXTROT_EDITOR
public:
	virtual void AddRefCount() override;
	virtual void SubtractRefCount() override;

#endif
};

namespace ChunkKey
{
	namespace FTSpriteAnimation
	{
		constexpr const char* FT_SPRITE_ANIMATION = "FTSpriteAnimation";
		constexpr const char* ANIM_TILEMAP_KEY	  = "TileMap Key";

		constexpr const char* FPS			= "FPS";
		constexpr const char* IS_REPEATED	= "Is Repeated";
		constexpr const char* MAX_FRAME_IDX = "Max Frame Index";
		constexpr const char* MIN_FRAME_IDX = "Min Frame Index";

	} // namespace FTSpriteAnimation
} // namespace ChunkKey