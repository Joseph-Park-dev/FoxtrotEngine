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

UINT FTSpriteAnimation::GetTexKey()
{
	return mTexKey;
}

UINT FTSpriteAnimation::GetTileMapKey()
{
	return mTileMapKey;
}

void FTSpriteAnimation::SetTexture(UINT key)
{
	FTBasicMeshGroup::SetTexture(key);
	mTexKey = key;
}

void FTSpriteAnimation::SetTileMapKey(UINT key)
{
	mTileMapKey = key;
}

FTSpriteAnimation::FTSpriteAnimation()
	: FTAnimation()
	, mTexKey		(ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	, mTileMapKey	(ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
{}

FTSpriteAnimation::FTSpriteAnimation(FTSpriteAnimation* other)
	: FTAnimation()
	, mTexKey		(other->mTexKey)
	, mTileMapKey	(other->mTileMapKey)
{
}

void FTSpriteAnimation::SaveProperties(std::ofstream& ofs, UINT key)
{
	FileIOHelper::BeginDataPackSave	(ofs, ChunkKey::FT_SPRITE_ANIMATION);

	FTResource::SaveProperties(ofs, key);
	FileIOHelper::SaveUnsignedInt	(ofs, ChunkKey::ANIM_TEXTURE_KEY, mTexKey);
	FileIOHelper::SaveUnsignedInt	(ofs, ChunkKey::ANIM_TILEMAP_KEY, mTileMapKey);

	FileIOHelper::EndDataPackSave	(ofs, ChunkKey::FT_SPRITE_ANIMATION);
}

UINT FTSpriteAnimation::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad	(ifs, ChunkKey::FT_SPRITE_ANIMATION);

	FileIOHelper::LoadUnsignedInt(ifs, mTileMapKey);
	FileIOHelper::LoadUnsignedInt(ifs, mTexKey);
	return FTResource::LoadProperties(ifs);
}