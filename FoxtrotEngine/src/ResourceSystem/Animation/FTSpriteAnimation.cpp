// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/Animation/FTSpriteAnimation.h"

#include "FileSystem/FileIOHelper.h"
#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "ResourceSystem/Animation/AnimationFrame.h"

#ifdef FOXTROT_EDITOR
	#include "Managers/AnimationManager.h"
#endif

const char* FTSpriteAnimation::GetTileDataKey()
{
	return mTileDataKey;
}

AnimationFrame* FTSpriteAnimation::GetFrame(int frameIdx)
{
	if (frameIdx < Meshes().size())
	{
		Mesh* mesh = Meshes().at(frameIdx);
		return static_cast<AnimationFrame*>(mesh);
	}
	return nullptr;
}

void FTSpriteAnimation::SetTileDataKey(const char* key)
{
	mTileDataKey = key;
}

FTSpriteAnimation::FTSpriteAnimation()
	: FTAnimation()
	, mTileDataKey(ChunkKey::NullVal::NULL_OBJECT)
{
}

FTSpriteAnimation::FTSpriteAnimation(FTSpriteAnimation* other)
	: FTAnimation()
	, mTileDataKey(other->mTileDataKey)
{
}

FTSpriteAnimation::~FTSpriteAnimation()
{
}

void FTSpriteAnimation::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::SpriteAnimation::FT_SPRITE_ANIMATION);

	FTAnimation::SaveProperties(ofs);
	FileIOHelper::SaveString(ofs, ChunkKey::SpriteAnimation::ANIM_TILEMAP_KEY, mTileDataKey);

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::SpriteAnimation::FT_SPRITE_ANIMATION);
}

void FTSpriteAnimation::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SpriteAnimation::FT_SPRITE_ANIMATION);

	FileIOHelper::LoadBasicString(ifs, mTileDataKey);
	FTAnimation::LoadProperties(ifs);
}