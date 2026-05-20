// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component/SpriteRenderer.h"

#include "Actor/IActor.h"
#include "Actor/Transform.h"
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
	#include <EditorCamera.h>

	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include <ImGuiFileDialog/ImGuiFileDialog.h>
#endif // FOXTROT_EDITOR

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	void SpriteRenderer::Initialize()
	{
		D3D11Component::Initialize();
	}

	void SpriteRenderer::Setup()
	{
		D3D11Component::Setup();
	}

	void SpriteRenderer::ProcessInput(IInputDevice* inputDevice)
	{
	}

	void SpriteRenderer::Update(float deltaTime)
	{
	}

	void SpriteRenderer::LateUpdate(float deltaTime)
	{
	}

	void SpriteRenderer::Render(D3D11::D3D11Renderer* renderer, D3D11::Camera* camInst)
	{
		if (mSprite)
		{
			mSprite->UpdateConstantBuffers(renderer, GetOwner()->GetTransform(), camInst, mMaterial);
			mSprite->Render(renderer, GetOwner()->GetTransform(), camInst, mPSO, mMaterial);
		}
	}

	void SpriteRenderer::CloneTo(Core::IActor* actor)
	{
		SpriteRenderer* newComp = DBG_NEW SpriteRenderer(actor, GetUpdateOrder());
		newComp->mSprite		= this->mSprite;
		newComp->mPSO			= this->mPSO;
		newComp->mMaterial		= this->mMaterial;
	}

	SpriteRenderer::SpriteRenderer(IActor* owner, int updateOrder)
		: D3D11Component(owner, updateOrder)
		, mSprite(nullptr)
		, mMaterial(nullptr)
	{
		mPSO = D3D11::ResourceManager::GetInstance()->GetPSO(ChunkKey::SpriteRenderer::PSO);
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
		D3D11::D3D11Component::SaveProperties(ofs);
		FileIOHelper::SaveString(ofs, ChunkKey::SpriteRenderer::SPRITE, mSprite->GetFileName());
		FileIOHelper::SaveString(ofs, ChunkKey::SpriteRenderer::MATERIAL, mMaterial->GetFileName());
	}

	void SpriteRenderer::LoadProperties(std::ifstream& ifs)
	{
		Common::FTDS::String matKey;
		FileIOHelper::LoadBasicString(ifs, matKey);

		Common::FTDS::String spriteKey;
		FileIOHelper::LoadBasicString(ifs, spriteKey);
		D3D11::D3D11Component::LoadProperties(ifs);

		mSprite	  = D3D11::ResourceManager::GetInstance()->GetSprite(spriteKey);
		mMaterial = D3D11::ResourceManager::GetInstance()->GetMaterial(matKey);
	}

#ifdef FOXTROT_EDITOR
	FTSprite*& SpriteRenderer::Sprite()
	{
		return mSprite;
	}
	FTMaterial*& SpriteRenderer::Material()
	{
		return mMaterial;
	}
#endif // FOXTROT_EDITOR
} // namespace D3D11