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
#include "ResourceSystem/GeometryGenerator.h"
#include "Managers/ResourceManager.h"

#ifdef FOXTROT_EDITOR
	#include "Managers/AnimationManager.h"
	#include "EditorResourceManager.h"
	#include "ResourceSystem/FTShaders/FTVertexShader.h"
#endif

void FTSpriteAnimation::Initialize(const Tile* tiles, ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	FTDS::DynamicArray<FTMeshData*> meshDataBuf;
	GeometryGenerator::MakeSpriteAnimation(
		meshDataBuf, tiles, mMinFrameIdx, mMaxFrameIdx);
	FTMeshGroup::Initialize(std::move(meshDataBuf), device, context);
}

void FTSpriteAnimation::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);

	FTMeshGroup::SaveProperties(ofs);
	FileIOHelper::SaveFloat(ofs, ChunkKey::FTSpriteAnimation::FPS, mFPS);
	FileIOHelper::SaveBool(ofs, ChunkKey::FTSpriteAnimation::IS_REPEATED, mIsRepeated);
	FileIOHelper::SaveInt(ofs, ChunkKey::FTSpriteAnimation::MAX_FRAME_IDX, mMaxFrameIdx);
	FileIOHelper::SaveInt(ofs, ChunkKey::FTSpriteAnimation::MIN_FRAME_IDX, mMinFrameIdx);
	FileIOHelper::SaveString(ofs, ChunkKey::FTSpriteAnimation::ANIM_TILEMAP_KEY, mAtlas->GetFileName());

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);
}

void FTSpriteAnimation::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTSpriteAnimation::FT_SPRITE_ANIMATION);

	FTDS::String atlasKey;
	FileIOHelper::LoadBasicString(ifs, atlasKey);
	FileIOHelper::LoadInt(ifs, mMinFrameIdx);
	FileIOHelper::LoadInt(ifs, mMaxFrameIdx);
	FileIOHelper::LoadBool(ifs, mIsRepeated);
	FileIOHelper::LoadInt(ifs, mFPS);
	FTMeshGroup::LoadProperties(ifs);

	mAtlas =  ResourceManager::GetInstance()->GetLoadedText(atlasKey);
}

const int FTSpriteAnimation::GetFPS() const
{
	return mFPS;
}

const int FTSpriteAnimation::GetMaxFrameIdx() const { return mMaxFrameIdx; }
const int FTSpriteAnimation::GetMinFrameIdx() const { return mMinFrameIdx; }

FTSpriteAnimation::FTSpriteAnimation(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTMeshGroup(resDef, renderer)
	, mMinFrameIdx(0)
	, mMaxFrameIdx(0)
	, mFPS(30)
	, mIsRepeated(true)
{
}

FTSpriteAnimation::~FTSpriteAnimation()
{
}

void FTSpriteAnimation::Process(FoxtrotRenderer* renderer)
{
	if (IsProcessed())
		return;

	std::ifstream ifs(GetRelativePath().C_Str());
	this->LoadProperties(ifs);

	//Tile generate code here;

	FTResource::Process();
}

#ifdef FOXTROT_EDITOR
void FTSpriteAnimation::AddRefCount()
{
	mAtlas->AddRefCount();
	FTMeshGroup::AddRefCount();
}

void FTSpriteAnimation::SubtractRefCount()
{
	mAtlas->SubtractRefCount();
	FTMeshGroup::SubtractRefCount();
}
#endif