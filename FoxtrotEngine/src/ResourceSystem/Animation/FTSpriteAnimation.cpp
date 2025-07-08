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
#include "ResourceSystem/FTSpriteSheet.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Managers/ResourceManager.h"

#ifdef FOXTROT_EDITOR
	#include "Managers/AnimationManager.h"
	#include "EditorResourceManager.h"
	#include "ResourceSystem/FTShaders/FTVertexShader.h"
#endif

FTDS::String& FTSpriteAnimation::GetTileDataKey()
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

void FTSpriteAnimation::SetTileDataKey(FTDS::String& key)
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

void FTSpriteAnimation::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	// This if statement will be triggered only on Editor
	// (When loading all assets from Asset folder)
	if (this->GetTileDataKey().Equal(ChunkKey::NullVal::NULL_OBJECT))
	{
		std::ifstream ifs(this->RelativePath().C_Str());
		this->LoadProperties(ifs);
	}

#ifdef FOXTROT_EDITOR
	FTSpriteSheet* spriteSheet = EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(this->GetTileDataKey());
#else
	FTSpriteSheet* spriteSheet = ResourceManager::GetInstance()->GetLoadedSpriteSheet(this->GetTileDataKey());
#endif // FOXTROT_EDITOR

	if (!spriteSheet)
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to load spritesheet");
		return;
	}

	this->SetTexture();

	FoxtrotRenderer*		renderer = coreInst->GetGameRenderer();
	std::vector<FTMeshData> meshDataBuf;
	GeometryGenerator::MakeSpriteAnimation(
		meshDataBuf, spriteSheet->GetTiles(), this->GetMinFrameIdx(), this->GetMaxFrameIdx());
	this->Initialize(std::move(meshDataBuf), renderer->GetDevice(), renderer->GetContext());

	this->SetIsProcessed(true);
}

#ifdef FOXTROT_EDITOR
void FTSpriteAnimation::AddRefCount()
{
	if (GetTexture())
		GetTexture()->AddRefCount();
	for (FTMaterial* mat : Materials())
		mat->AddRefCount();
	
	if (GetVertexShader())
		GetVertexShader()->AddRefCount();
	if (GetPixelShader())
		GetPixelShader()->AddRefCount();

	FTSpriteSheet* sheet =  EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(this->GetTileDataKey());
	if (sheet)
		sheet->AddRefCount();

	FTResource::AddRefCount();
}

//void FTSpriteAnimation::SubtractRefCount()
//{
//	if (GetTexture())
//		GetTexture()->SubtractRefCount();
//	for (FTMaterial* mat : Materials())
//		mat->SubtractRefCount();
//
//	if (GetVertexShader())
//		GetVertexShader()->SubtractRefCount();
//	if (GetPixelShader())
//		GetPixelShader()->SubtractRefCount();
//
//	FTSpriteSheet* sheet = EditorResourceManager::GetInstance()->GetLoadedSpriteSheet(this->GetTileDataKey());
//	if (sheet)
//		sheet->SubtractRefCount();
//}
#endif