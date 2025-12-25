// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteRenderer.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Core/FTCore.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"
#include "Math/FTMath.h"
#include "Renderer/Camera.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/FTMeshGroup.h"
#include "ResourceSystem/FTSprite/FTSprite.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTMeshData.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "Scenes/Scene.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "EditorUtils.h"
	#include <EditorCamera.h>

	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include <imgui/ImGuiFileDialog/ImGuiFileDialog.h>
#endif // FOXTROT_EDITOR

void SpriteRenderer::Initialize(FTCore* coreInstance)
{
	Component::Initialize(coreInstance);
}

void SpriteRenderer::Render(FoxtrotRenderer* renderer)
{
	if (mSprite)
		mSprite->Render(renderer, GetOwner()->GetTransform(), Camera::GetInstance(), mVS, mGS, mPS, mMaterial);
}

void SpriteRenderer::CloneTo(Actor* actor)
{
	SpriteRenderer* newComp = DBG_NEW SpriteRenderer(actor, GetUpdateOrder());
	newComp->mSprite		= this->mSprite;
	newComp->mVS			= this->mVS;
	newComp->mGS			= this->mGS;
	newComp->mPS			= this->mPS;
	newComp->mMaterial		= this->mMaterial;
}

SpriteRenderer::SpriteRenderer(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mSprite(nullptr)
	, mMaterial(nullptr)
{
	FTDS::String key = Path::SpriteRenderer::VS;
	mVS				 = ResourceManager::GetInstance()->GetLoadedVertexShader(key);

	key = Path::SpriteRenderer::GS;
	mGS = ResourceManager::GetInstance()->GetLoadedGeometryShader(key);

	key = Path::SpriteRenderer::PS;
	mPS = ResourceManager::GetInstance()->GetLoadedPixelShader(key);
}

FTSprite* SpriteRenderer::GetSprite() const
{
	return mSprite;
}

void SpriteRenderer::SetSprite(FTSprite* sprite)
{
	mSprite = sprite;
}

FTVertexShader* SpriteRenderer::GetVS() const
{
	return mVS;
}

FTGeometryShader* SpriteRenderer::GetGS() const
{
	return mGS;
}

FTPixelShader* SpriteRenderer::GetPS() const
{
	return mPS;
}

FTMaterial* SpriteRenderer::GetMaterial() const
{
	return mMaterial;
}

void SpriteRenderer::SetVS(FTVertexShader* vs) { mVS = vs; }
void SpriteRenderer::SetGS(FTGeometryShader* gs) { mGS = gs; }
void SpriteRenderer::SetPS(FTPixelShader* ps) { mPS = ps; }
void SpriteRenderer::SetMaterial(FTMaterial* mat) { mMaterial = mat; }

void SpriteRenderer::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::SaveString(ofs, ChunkKey::SpriteRenderer::SPRITE, mSprite->GetFileName());
	FileIOHelper::SaveString(ofs, ChunkKey::SpriteRenderer::MATERIAL, mMaterial->GetFileName());
}

void SpriteRenderer::LoadProperties(std::ifstream& ifs)
{
	FTDS::String matKey;
	FileIOHelper::LoadBasicString(ifs, matKey);

	FTDS::String spriteKey;
	FileIOHelper::LoadBasicString(ifs, spriteKey);
	Component::LoadProperties(ifs);

	mSprite	  = ResourceManager::GetInstance()->GetLoadedSprite(spriteKey);
	mMaterial = ResourceManager::GetInstance()->GetLoadedMaterial(matKey);
}

#ifdef FOXTROT_EDITOR
void SpriteRenderer::EditorUIUpdate()
{
	Component::EditorUIUpdate();

	if (mSprite)
		mSprite->UpdateUI();

	FTEditorUtils::DisplayResSelection(
		"Select Sprite",
		ResourceManager::GetInstance()->GetSprites(),
		mSprite);

	if (mMaterial)
		mMaterial->UpdateUI();

	FTEditorUtils::DisplayResSelection(
		"Select Material",
		ResourceManager::GetInstance()->GetMaterials(),
		mMaterial);
}
#endif // FOXTROT_EDITOR