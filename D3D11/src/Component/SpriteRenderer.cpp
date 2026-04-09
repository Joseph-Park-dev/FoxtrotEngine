// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "SpriteRenderer.h"

#include "Actor/Actor.h"
#include "Actor/Transform.h"
#include "FTCore.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Manager/ResourceManager.h"
#include "Math/FTMath.h"
#include "Renderer/Camera.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "ResourceSystem/Mesh/FTMeshGroup.h"
#include "ResourceSystem/Sprite/FTSprite.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Mesh/FTMeshData.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/D3D11PSO.h"
#include "Scene/Scene.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "EditorUtils.h"
	#include <EditorCamera.h>

	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include <imgui/ImGuiFileDialog/ImGuiFileDialog.h>
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Core;
	void SpriteRenderer::Initialize()
	{
		D3D11Component::Initialize();
	}

	void SpriteRenderer::Setup()
	{
		D3D11Component::Setup();
	}

	void SpriteRenderer::ProcessInput(FTInputDevice* inputDevice)
	{
	}

	void SpriteRenderer::Update(float deltaTime)
	{
	}

	void SpriteRenderer::LateUpdate(float deltaTime)
	{
	}

	void SpriteRenderer::Render(Core::FoxtrotRenderer* renderer)
	{
		D3D11Renderer* rend = static_cast<D3D11Renderer*>(renderer);
		if (mSprite)
		{
			mSprite->UpdateConstantBuffers(rend, GetOwner()->GetTransform(), Camera::GetInstance(), mMaterial);
			mSprite->Render(rend, GetOwner()->GetTransform(), Camera::GetInstance(), mPSO, mMaterial);
		}
	}

	void SpriteRenderer::CloneTo(Actor* actor)
	{
		SpriteRenderer* newComp = DBG_NEW SpriteRenderer(GetPlugin(), actor, GetUpdateOrder());
		newComp->mSprite		= this->mSprite;
		newComp->mPSO			= this->mPSO;
		newComp->mMaterial		= this->mMaterial;
	}

	SpriteRenderer::SpriteRenderer(Plugin* plugin, Actor* owner, int updateOrder)
		: D3D11Component(plugin, owner, updateOrder)
		, mSprite(nullptr)
		, mMaterial(nullptr)
	{
		mPSO = D3D11::GET_RES(D3D11PSO, ChunkKey::SpriteRenderer::PSO);
	}

	FTSprite* SpriteRenderer::GetSprite() const
	{
		return mSprite;
	}

	FTMaterial* SpriteRenderer::GetMaterial() const
	{
		return mMaterial;
	}

	D3D11PSO* SpriteRenderer::GetPSO() const
	{
		return mPSO;
	}

	void SpriteRenderer::SetSprite(FTSprite* sprite) { mSprite = sprite; }
	void SpriteRenderer::SetMaterial(FTMaterial* mat) { mMaterial = mat; }
	void SpriteRenderer::SetPSO(D3D11PSO* pso) { mPSO = pso; }

	void SpriteRenderer::SaveProperties(std::ofstream& ofs)
	{
		Component::SaveProperties(ofs);
		FileIOHelper::SaveString(ofs, ChunkKey::SpriteRenderer::SPRITE, D3D11::RES_NAME(FTSprite, mSprite));
		FileIOHelper::SaveString(ofs, ChunkKey::SpriteRenderer::MATERIAL, D3D11::RES_NAME(FTMaterial, mMaterial));
	}

	void SpriteRenderer::LoadProperties(std::ifstream& ifs)
	{
		FTDS::String matKey;
		FileIOHelper::LoadBasicString(ifs, matKey);

		FTDS::String spriteKey;
		FileIOHelper::LoadBasicString(ifs, spriteKey);
		Component::LoadProperties(ifs);

		mSprite	  = D3D11::GET_RES(FTSprite, spriteKey);
		mMaterial = D3D11::GET_RES(FTMaterial, matKey);
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
	void SpriteRenderer::EditorRender(FoxtrotRenderer* renderer)
	{
		if (mSprite)
		{
			mSprite->UpdateConstantBuffers(renderer, GetOwner()->GetTransform(), Camera::GetInstance(), mMaterial);
			mSprite->Render(renderer, GetOwner()->GetTransform(), EditorCamera::GetInstance(), mPSO, mMaterial);
		}
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11