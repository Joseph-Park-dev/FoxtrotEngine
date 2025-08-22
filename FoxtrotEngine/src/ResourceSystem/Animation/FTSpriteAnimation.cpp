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

AnimationFrame* FTSpriteAnimation::GetFrame(int frameIdx)
{
	if (frameIdx < Meshes()->GetSize())
	{
		Mesh* mesh = Meshes()->At(frameIdx);
		return static_cast<AnimationFrame*>(mesh);
	}
	return nullptr;
}

void FTSpriteAnimation::SetSpriteSheet(FTSpriteSheet* sheet)
{
	mSpriteSheet = sheet;
}

FTSpriteAnimation::FTSpriteAnimation()
	: FTAnimation()
	, mSpriteSheet(nullptr)
{
}

FTSpriteAnimation::FTSpriteAnimation(FTSpriteAnimation* other)
	: FTAnimation()
	, mSpriteSheet(other->mSpriteSheet)
{
}

FTSpriteAnimation::~FTSpriteAnimation()
{
}

void FTSpriteAnimation::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::SpriteAnimation::FT_SPRITE_ANIMATION);

	FTAnimation::SaveProperties(ofs);
	FileIOHelper::SaveString(ofs, ChunkKey::SpriteAnimation::ANIM_TILEMAP_KEY, mSpriteSheet->FileName());

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::SpriteAnimation::FT_SPRITE_ANIMATION);
}

void FTSpriteAnimation::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SpriteAnimation::FT_SPRITE_ANIMATION);

	FileIOHelper::LoadResource(ifs, mSpriteSheet, ResourceManager::GetInstance()->GetSpriteSheets());
	FTAnimation::LoadProperties(ifs);
}

void FTSpriteAnimation::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	std::ifstream ifs(this->RelativePath().C_Str());
	this->LoadProperties(ifs);

	if (!mSpriteSheet)
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to load spritesheet");
		return;
	}

	FoxtrotRenderer*		renderer = coreInst->GetGameRenderer();
	FTDS::DynamicArray<FTMeshData*> meshDataBuf;
	GeometryGenerator::MakeSpriteAnimation(
		meshDataBuf, mSpriteSheet->GetTiles(), this->GetMinFrameIdx(), this->GetMaxFrameIdx());
	this->Initialize(std::move(meshDataBuf), renderer->GetDevice(), renderer->GetContext());

	this->SetIsProcessed(true);
}

#ifdef FOXTROT_EDITOR
void FTSpriteAnimation::AddRefCount()
{
	if (mSpriteSheet)
		mSpriteSheet->AddRefCount();
	FTBasicMeshGroup::AddRefCount();
}

void FTSpriteAnimation::SubtractRefCount()
{
	if (mSpriteSheet)
		mSpriteSheet->SubtractRefCount();
	FTBasicMeshGroup::SubtractRefCount();
}

// void FTSpriteAnimation::SubtractRefCount()
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
// }
#endif