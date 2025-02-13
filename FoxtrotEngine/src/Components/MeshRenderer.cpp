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
#include "ResourceSystem/MeshData.h"
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "Renderer/Camera.h"
#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Core/TemplateFunctions.h"
#include "Managers/ResourceManager.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"

#ifdef FOXTROT_EDITOR
	#include "FTCoreEditor.h"
	#include "EditorUtils.h"
#endif // FOXTROT_EDITOR

using DXMatrix = DirectX::SimpleMath::Matrix;

void MeshRenderer::Initialize(FTCore* coreInstance)
{
	mRenderer = coreInstance->GetGameRenderer();
	if (mMeshKey != ChunkKeys::VALUE_NOT_ASSIGNED)
	{
		this->InitializeMesh();
		if (mTexKey != ChunkKeys::VALUE_NOT_ASSIGNED)
			SetTexture();
	}
	// mMeshGroup = DBG_NEW FTBasicMeshGroup;
	Component::Initialize(coreInstance);
}

void MeshRenderer::Update(float deltaTime)
{
	if (mMeshGroup)
	{
		UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance());
		UpdateBuffers();
	}
}

void MeshRenderer::Render(FoxtrotRenderer* renderer)
{
	if (mMeshGroup)
	{
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		mMeshGroup->Render(renderer, mTexture);
	}
}

void MeshRenderer::CloneTo(Actor* actor)
{
	MeshRenderer* newComp = DBG_NEW MeshRenderer(actor, GetUpdateOrder());
	newComp->mMeshKey	  = this->mMeshKey;
	newComp->mTexKey	  = this->mTexKey;
}

bool MeshRenderer::InitializeMesh()
{
	if (mMeshKey != ChunkKeys::VALUE_NOT_ASSIGNED)
	{
		std::vector<MeshData>& meshData = ResourceManager::GetInstance()->GetLoadedMeshes(mMeshKey);
		if (!mMeshGroup)
			mMeshGroup = DBG_NEW FTBasicMeshGroup;
		mMeshGroup->Initialize(meshData, mRenderer->GetDevice(), mRenderer->GetContext());

		if (!mMaterial)
			mMaterial = DBG_NEW FTMaterial;
		if (!mMeshGroup)
		{
			LogString("ERROR: MeshRenderer::InitializeMesh() -> Mesh Init failed.\n");
			return false;
		}
		return true;
	}
	else
	{
		LogString("ERROR: MeshRenderer::InitializeMesh() -> Key doesn't exist.\n");
		return false;
	}
}

bool MeshRenderer::InitializeMesh(UINT key)
{
	mMeshKey = key;
	MeshRenderer::InitializeMesh();
	return mMeshGroup != nullptr;
}

bool MeshRenderer::InitializeMesh(MeshData& meshData)
{
	if (!mMeshGroup)
		mMeshGroup = DBG_NEW FTBasicMeshGroup;
	if (!mMaterial)
		mMaterial = DBG_NEW FTMaterial;
	std::vector<MeshData> meshes = { meshData };
	mMeshGroup->Initialize(meshes, mRenderer->GetDevice(), mRenderer->GetContext());
	if (!mMeshGroup)
	{
		LogString("ERROR: MeshRenderer::InitializeMesh() -> Mesh Init failed.\n");
		return false;
	}
	return true;
}

bool MeshRenderer::InitializeMesh(std::vector<MeshData>& meshData)
{
	if (!mMeshGroup)
		mMeshGroup = DBG_NEW FTBasicMeshGroup;
	if (!mMaterial)
		mMaterial = DBG_NEW FTMaterial;
	mMeshGroup->Initialize(meshData, mRenderer->GetDevice(), mRenderer->GetContext());
	if (!mMeshGroup)
	{
		LogString("ERROR: MeshRenderer::InitializeMesh() -> Mesh Init failed.\n");
		return false;
	}
	return true;
}

bool MeshRenderer::SetTexture()
{
	if (mTexKey == ChunkKeys::VALUE_NOT_ASSIGNED)
	{
		printf("ERROR: MeshRenderer::SetTexture() -> TexKey not assigned.\n");
		return false;
	}
	mTexture = ResourceManager::GetInstance()->GetLoadedTexture(mTexKey);
	if (!mTexture)
		printf("ERROR: MeshRenderer::SetTexture() -> Cannot set texture %d, returning nullptr.\n", mTexKey);
	return mTexture != nullptr;
}

void MeshRenderer::UpdateMesh(Transform* transform, Camera* camInst)
{
	if (mMeshGroup)
	{
		// Model Transformation
		Matrix&& modelMat		 = std::move(CalcModelMat(transform));
		Matrix	 invTransposeMat = modelMat.Transpose();
		invTransposeMat.Translation(Vector3(0.0f));
		invTransposeMat = invTransposeMat.Transpose().Invert();

		// View Transformation
		Matrix&& viewMat  = camInst->GetViewRow();
		Vector3	 eyeWorld = Vector3::Transform(Vector3(0.0f), viewMat.Invert());

		// Project Transformation
		Matrix&& projMat = std::move(camInst->GetProjRow());

		for (Mesh* mesh : mMeshGroup->GetMeshes())
		{
			mesh->VertexConstantData.model		  = modelMat.Transpose();
			mesh->VertexConstantData.view		  = viewMat.Transpose();
			mesh->VertexConstantData.projection	  = projMat.Transpose();
			mesh->VertexConstantData.invTranspose = std::move(invTransposeMat);

			mesh->PixelConstantData.EyeWorld   = eyeWorld;
			mesh->PixelConstantData.UseTexture = true;

			mesh->PixelConstantData.Material.Diffuse  = mMaterial->Diffuse;
			mesh->PixelConstantData.Material.Specular = mMaterial->Specular;

			for (size_t i = 0; i < Light::TYPE::END; ++i)
			{
				if (LightManager::GetInstance()->GetType(0) == (Light::TYPE)i)
					mesh->PixelConstantData.Lights[i] = LightManager::GetInstance()->GetLight(0);
				else
					mesh->PixelConstantData.Lights[i].Strength *= 0.0f;
			}
		}
	}
}

void MeshRenderer::UpdateBuffers()
{
	if (mMeshGroup)
		mMeshGroup->UpdateConstantBuffers(mRenderer->GetDevice(), mRenderer->GetContext());
}

Matrix MeshRenderer::CalcModelMat(Transform* transform)
{
	int				  dir		   = transform->GetRightward().x;
	FTVector3		  scale		   = transform->GetScale();
	DirectX::XMFLOAT3 scaleWithDir = DirectX::XMFLOAT3(scale.x, scale.y, scale.z);
	return DXMatrix::CreateScale(scaleWithDir) *
		DXMatrix::CreateRotationX(transform->GetRotation().x) *
		DXMatrix::CreateRotationY(transform->GetRotation().y) *
		DXMatrix::CreateRotationZ(transform->GetRotation().z) *
		DXMatrix::CreateTranslation(transform->GetWorldPosition().GetDXVec3());
}

MeshRenderer::MeshRenderer(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mMeshGroup(nullptr)
	, mTexture(nullptr)
	, mMaterial(nullptr)
	, mRenderer(nullptr)
	, mMeshKey(ChunkKeys::VALUE_NOT_ASSIGNED)
	, mTexKey(ChunkKeys::VALUE_NOT_ASSIGNED)
{
}

MeshRenderer::~MeshRenderer()
{
	if (mMeshGroup)
	{
		delete mMeshGroup;
		mMeshGroup = nullptr;
	}
	if (mMaterial)
	{
		delete mMaterial;
		mMaterial = nullptr;
	}
}

void MeshRenderer::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::MESH_KEY, mMeshKey);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::TEXTURE_KEY, mTexKey);
}

void MeshRenderer::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadUnsignedInt(ifs, mTexKey);
	FileIOHelper::LoadUnsignedInt(ifs, mMeshKey);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void MeshRenderer::EditorUpdate(float deltaTime)
{
	Update(deltaTime);
}

void MeshRenderer::EditorUIUpdate()
{
	CHECK_RENDERER(GetRenderer());
	if (ImGui::Button("Add Cube"))
	{
		MeshData meshData =
			ResourceManager::GetInstance()->GetLoadedPrimitive(ChunkKeys::PRIMITIVE_BOX);
		InitializeMesh(meshData);
		LogString("Cube added");
	}
	OnConfirmUpdate();
	UpdateSprite();
}

void MeshRenderer::OnConfirmUpdate()
{
	if (ImGui::Button("Update"))
	{
		SetTexture();
	}
}

void MeshRenderer::OnResetTexture()
{
	if (ImGui::Button("Reset"))
	{
		GetTexture()->ReleaseTexture();
		SetTexKey(ChunkKeys::VALUE_NOT_ASSIGNED);
	}
}

void MeshRenderer::UpdateSprite()
{
	std::string currentSprite = "No sprite has been assigned";
	if (mTexKey != ChunkKeys::VALUE_NOT_ASSIGNED)
	{
		currentSprite =
			"Current sprite : \n" + ResourceManager::GetInstance()->GetLoadedTexture(GetTexKey())->GetRelativePath();
		if (mMeshGroup && mTexture)
		{
			ImVec2 size = ImVec2(100, 100);
			ImGui::Image((ImTextureID)GetTexture()->GetResourceView().Get(), size);
		}
	}
	ImGui::Text(currentSprite.c_str());

	UINT key =
		FTEditorUtils::DisplayResSelection<FTTexture>(
			"Select Sprite",
			ResourceManager::GetInstance()->GetTexturesMap());
	if (key != ChunkKeys::VALUE_NOT_ASSIGNED)
		mTexKey = key;
}

void MeshRenderer::UpdateSprite(UINT& key)
{
	std::string currentSprite = {};
	if (key != ChunkKeys::VALUE_NOT_ASSIGNED)
	{
		FTTexture* sprite = ResourceManager::GetInstance()->GetLoadedTexture(key);
		currentSprite =
			"Current sprite : \n" + sprite->GetRelativePath();
		if (sprite)
		{
			ImVec2 size = ImVec2(100, 100);
			ImGui::Image((ImTextureID)sprite->GetResourceView().Get(), size);
		}
	}
	else
		currentSprite = "No sprite has been assigned";

	ImGui::Text(currentSprite.c_str());

	if (ImGui::Button("Select Sprite"))
	{
		IGFD::FileDialogConfig config;
		config.path				 = ".";
		config.countSelectionMax = 1;
		ImGuiFileDialog::Instance()->OpenDialog(
			"SelectSprite", "Select Sprite", FileTypes::TEXTURE, config);
		ImGui::OpenPopup("Select Sprite");
	}

	if (ImGui::BeginPopupModal("Select Sprite", NULL, ImGuiWindowFlags_MenuBar))
	{
		std::unordered_map<UINT, FTTexture*>& texturesMap =
			ResourceManager::GetInstance()->GetTexturesMap();
		if (ImGui::TreeNode("Selection State: Single Selection"))
		{
			UINT	   spriteKey = ChunkKeys::VALUE_NOT_ASSIGNED;
			static int selected	 = -1;
			int		   i		 = 0;
			for (auto iter = texturesMap.begin(); iter != texturesMap.end();
				 ++iter, ++i)
			{
				if (ImGui::Selectable((*iter).second->GetFileName().c_str(), selected == i))
				{
					spriteKey = (*iter).first;
					selected  = i;
				}
			}
			ImGui::TreePop();
			if (selected != -1)
			{
				key = spriteKey;
			}
		}
		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}
#endif // FOXTROT_EDITOR