// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/MeshRenderer.h"

#include <functional>
#include <directxtk/SimpleMath.h>

#include "Actors/Transform.h"
#include "Actors/Actor.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/FTMeshDataPack.h"
#include "ResourceSystem/FTMeshGroup.h"
#include "Renderer/Camera.h"
#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "Core/TemplateFunctions.h"
#include "Managers/ResourceManager.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Compare/StringEqual.h"

#ifdef FOXTROT_EDITOR
	#include "FTCoreEditor.h"
	#include "EditorUtils.h"
	#include "EditorCamera.h"
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

void MeshRenderer::Initialize(FTCore* coreInstance)
{
	mRenderer = coreInstance->GetGameRenderer();
	Component::Initialize(coreInstance);
}

void MeshRenderer::Render(FoxtrotRenderer* renderer)
{
	if (mMeshGroup)
	{
		UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance(), renderer);
		renderer->SwitchFillMode();
		mMeshGroup->Render(renderer, mTexture, mVS, mPS, mMaterial);
	}
}

void MeshRenderer::CloneTo(Actor* actor)
{
	MeshRenderer* newComp = DBG_NEW MeshRenderer(actor, GetUpdateOrder());
	newComp->mRenderer	  = this->mRenderer;
	newComp->mMeshGroup	  = this->mMeshGroup;
	newComp->mTexture	  = this->mTexture;
	newComp->mVS		  = this->mVS;
	newComp->mPS		  = this->mPS;
	newComp->mMaterial	  = this->mMaterial;
}

FoxtrotRenderer*  MeshRenderer::GetRenderer() const { return mRenderer; }
FTMeshGroup* MeshRenderer::GetMeshGroup() const { return mMeshGroup; }
FTTexture*		  MeshRenderer::GetTexture() const { return mTexture; }
FTVertexShader*	  MeshRenderer::GetVS() const { return mVS; }
FTPixelShader*	  MeshRenderer::GetPS() const { return mPS; }
FTMaterial*		  MeshRenderer::GetMaterial() const { return mMaterial; }

void MeshRenderer::SetRenderer(FoxtrotRenderer* renderer) { mRenderer = renderer; }
void MeshRenderer::SetMeshGroup(FTMeshGroup* meshGroup) { mMeshGroup = meshGroup; }
void MeshRenderer::SetTexture(FTTexture* tex) { mTexture = tex; }
void MeshRenderer::SetVS(FTVertexShader* vs) { mVS = vs; }
void MeshRenderer::SetPS(FTPixelShader* ps) { mPS = ps; }
void MeshRenderer::SetMaterial(FTMaterial* mat) { mMaterial = mat; }

void MeshRenderer::UpdateMesh(Transform* transform, Camera* camInst, FoxtrotRenderer* renderer)
{
	if (mMeshGroup)
	{
		mMeshGroup->CalcVCData(transform, camInst);
		mMeshGroup->UpdateConstantBuffers(
			renderer->GetDevice(),
			renderer->GetContext(),
			mMaterial);
	}
}

MeshRenderer::MeshRenderer(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mMeshGroup(nullptr)
	, mRenderer(nullptr)
	, mTexture(nullptr)
	, mVS(nullptr)
	, mPS(nullptr)
	, mMaterial(nullptr)
{
}

MeshRenderer::~MeshRenderer()
{
	if (mMeshGroup)
		mMeshGroup = nullptr;

	if (mMaterial)
		mMaterial = nullptr;
}

void MeshRenderer::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);

	if (mMeshGroup)
		FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::MESH_KEY, mMeshGroup->FileName());
	else
		FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::MESH_KEY, ChunkKey::NullVal::NULL_OBJECT);

	if (mTexture)
		FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::TEXTURE_KEY, mTexture->FileName());
	else
		FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::TEXTURE_KEY, ChunkKey::NullVal::NULL_OBJECT);

	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTMeshGroup::SHADER_KEY);
	FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::VS_KEY, mVS->FileName());
	FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::PS_KEY, mPS->FileName());
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTMeshGroup::SHADER_KEY);

	FileIOHelper::SaveString(ofs, ChunkKey::FTMeshGroup::MAT_KEY, mMaterial->FileName());
}

void MeshRenderer::LoadProperties(std::ifstream& ifs)
{
	FTDS::String keyCache; // Temporary space to store the loaded keys.

	// Load material.
	FileIOHelper::LoadBasicString(ifs, keyCache);
	mMaterial = ResourceManager::GetInstance()->GetLoadedMaterial(keyCache);

	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMeshGroup::SHADER_KEY);

	// Load Pixel Shader.
	FileIOHelper::LoadBasicString(ifs, keyCache);
	mPS = ResourceManager::GetInstance()->GetLoadedPixelShader(keyCache);

	// Load Vertex Shader.
	FileIOHelper::LoadBasicString(ifs, keyCache);
	mVS = ResourceManager::GetInstance()->GetLoadedVertexShader(keyCache);

	// Load Texture.
	FileIOHelper::LoadBasicString(ifs, keyCache);
	mTexture = ResourceManager::GetInstance()->GetLoadedTexture(keyCache);

	// Load MeshGroup.
	FileIOHelper::LoadBasicString(ifs, keyCache);
	mMeshGroup = ResourceManager::GetInstance()->GetLoadedMesh(keyCache);

	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void MeshRenderer::EditorRender(FoxtrotRenderer* renderer)
{
	if (mMeshGroup)
	{
		UpdateMesh(GetOwner()->GetTransform(), EditorCamera::GetInstance(), renderer);
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		mMeshGroup->Render(renderer, mTexture, mVS, mPS, mMaterial);
	}
}

void MeshRenderer::EditorUIUpdate()
{
	Component::EditorUIUpdate();

	CHECK_RENDERER(GetRenderer());

	if (!mMeshGroup)
		return;

	if (mTexture)
		mTexture->UpdateUI();
	FTEditorUtils::DisplayResSelection(
		"Select Texture",
		ResourceManager::GetInstance()->GetTextures(),
		mTexture);

	FTEditorUtils::DisplayResSelection(
		"Select VS",
		ResourceManager::GetInstance()->GetVertexShaders(),
		mVS);

	FTEditorUtils::DisplayResSelection(
		"Select PS",
		ResourceManager::GetInstance()->GetPixelShaders(),
		mPS);

	if (mMaterial)
		mMaterial->UpdateUI();
	FTEditorUtils::DisplayResSelection(
		"Select Material",
		ResourceManager::GetInstance()->GetMaterials(),
		mMaterial);
}
#endif // FOXTROT_EDITOR