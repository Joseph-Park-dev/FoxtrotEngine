// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/SpriteRenderer.h"

#include "Actor/Actor.h"
#include "Actor/Transform.h"
// #include "FTCore.h" // NULLPATH: File doesn't exist - FTCore class defined in FTCore.cpp
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Manager/ResourceManager.h"
#include "FTMath.h"
#include "Renderer/Camera.h"
#include "Utility/D3D11Utils.h"
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
	#include "EditorCamera.h"

	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui/imgui.h>
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

	void SpriteRenderer::Render(D3D11::D3D11Renderer* renderer)
	{
		if (mSprite)
		{
			mSprite->UpdateConstantBuffers(renderer, GetOwner()->GetTransform(), Camera::GetInstance(), mMaterial);
			mSprite->Render(renderer, GetOwner()->GetTransform(), Camera::GetInstance(), mPSO, mMaterial);
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
		mPSO = D3D11::ResourceManager::GetInstance()->GetResource<D3D11PSO>(ChunkKey::SpriteRenderer::PSO);
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
		FileIOHelper::SaveString(ofs, ChunkKey::SpriteRenderer::SPRITE, mSprite->GetFileName());
		FileIOHelper::SaveString(ofs, ChunkKey::SpriteRenderer::MATERIAL, mMaterial->GetFileName());
	}

	void SpriteRenderer::LoadProperties(std::ifstream& ifs)
	{
		Common::FTDS::String matKey;
		FileIOHelper::LoadBasicString(ifs, matKey);

		Common::FTDS::String spriteKey;
		FileIOHelper::LoadBasicString(ifs, spriteKey);
		Component::LoadProperties(ifs);

		mSprite	  = D3D11::ResourceManager::GetInstance()->GetResource<FTSprite>(spriteKey);
		mMaterial = D3D11::ResourceManager::GetInstance()->GetResource<FTMaterial>(matKey);
	}

#ifdef FOXTROT_EDITOR
	void SpriteRenderer::EditorUIUpdate()
	{
		Component::EditorUIUpdate();

		if (mSprite)
			mSprite->UpdateUI();

		Editor::DisplayResSelection(
			"Select Sprite",
			&ResourceManager::GetInstance()->GetResMap<FTSprite>(),
			mSprite);

		if (mMaterial)
			mMaterial->UpdateUI();

		Editor::DisplayResSelection(
			"Select Material",
			&ResourceManager::GetInstance()->GetResMap<FTMaterial>(),
			mMaterial);
	}
	void SpriteRenderer::EditorRender(D3D11::D3D11Renderer* renderer)
	{
		if (mSprite)
		{
			mSprite->UpdateConstantBuffers(renderer, GetOwner()->GetTransform(), Camera::GetInstance(), mMaterial);
			mSprite->Render(renderer, GetOwner()->GetTransform(), Editor::EditorCamera::GetInstance(), mPSO, mMaterial);
		}
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11