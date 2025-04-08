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

UINT FTSpriteAnimation::GetTileMapKey()
{
	return mTileMapKey;
}

AnimationFrame* FTSpriteAnimation::GetFrame(int frameIdx)
{
	Mesh* mesh = Meshes().at(frameIdx);
	return static_cast<AnimationFrame*>(mesh);
}

void FTSpriteAnimation::SetTileMapKey(UINT key)
{
	mTileMapKey = key;
}

FTSpriteAnimation::FTSpriteAnimation()
	: FTAnimation()
	, mTileMapKey	(ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
{}

FTSpriteAnimation::FTSpriteAnimation(FTSpriteAnimation* other)
	: FTAnimation()
	, mTileMapKey	(other->mTileMapKey)
{
}

FTSpriteAnimation::~FTSpriteAnimation()
{ }

void FTSpriteAnimation::SaveProperties(std::ofstream& ofs, UINT key)
{
	FileIOHelper::BeginDataPackSave	(ofs, ChunkKey::SpriteAnimation::FT_SPRITE_ANIMATION);

	FTAnimation::SaveProperties(ofs, key);
	FileIOHelper::SaveUnsignedInt	(ofs, ChunkKey::SpriteAnimation::ANIM_TILEMAP_KEY, mTileMapKey);

	FileIOHelper::EndDataPackSave	(ofs, ChunkKey::SpriteAnimation::FT_SPRITE_ANIMATION);
}

UINT FTSpriteAnimation::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad	(ifs, ChunkKey::SpriteAnimation::FT_SPRITE_ANIMATION);

	FileIOHelper::LoadUnsignedInt(ifs, mTileMapKey);
	return FTAnimation::LoadProperties(ifs);
}