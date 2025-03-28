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
#include "ResourceSystem/FTBasicMeshGroup.h"
#include "Renderer/Camera.h"
#include "Renderer/FoxtrotRenderer.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "Core/TemplateFunctions.h"
#include "Managers/ResourceManager.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Compare/StringEqual.h"

#ifdef FOXTROT_EDITOR
	#include "FTCoreEditor.h"
	#include "EditorUtils.h"
#include "EditorCamera.h"
#endif // FOXTROT_EDITOR

void MeshRenderer::Initialize(FTCore* coreInstance)
{
	mRenderer = coreInstance->GetGameRenderer();
	if (mMeshKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	{
		this->InitializeMesh();
		if (mTexKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
			mMeshGroup->SetTexture(mTexKey);
	}
	// mMeshGroup = DBG_NEW FTBasicMeshGroup;
	Component::Initialize(coreInstance);
}

void MeshRenderer::Render(FoxtrotRenderer* renderer)
{
	if (mMeshGroup)
		UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance());

	if (mMeshGroup)
	{
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		mMeshGroup->Render(renderer);
	}
}

void MeshRenderer::CloneTo(Actor* actor)
{
	MeshRenderer* newComp = DBG_NEW MeshRenderer(actor, GetUpdateOrder());
	newComp->mMeshKey	  = this->mMeshKey;
	newComp->mTexKey	  = this->mTexKey;
	newComp->mMeshGroup->SetDrawNormal(this->mMeshGroup->GetDrawNormal());
}

FoxtrotRenderer*  MeshRenderer::GetRenderer() const { return mRenderer; }
const UINT		  MeshRenderer::GetMeshKey() const { return mMeshKey; }
const UINT		  MeshRenderer::GetTexKey() const { return mTexKey; }
FTBasicMeshGroup* MeshRenderer::GetMeshGroup() const { return mMeshGroup; }
FTTexture*		  MeshRenderer::GetTexture() const { return mMeshGroup->GetTexture(); }

void MeshRenderer::SetRenderer(FoxtrotRenderer* renderer) { mRenderer = renderer; }
void MeshRenderer::SetMeshKey(const UINT key) { mMeshKey = key; }
void MeshRenderer::SetTexKey(const UINT key) { mTexKey = key; }
void MeshRenderer::SetMeshGroup(FTBasicMeshGroup* meshGroup) { mMeshGroup = meshGroup; }

bool MeshRenderer::InitializeMesh()
{
	if (mMeshKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	{
		FTMeshDataPack* meshData = ResourceManager::GetInstance()->GetLoadedMeshData(mMeshKey);
		if (!mMeshGroup)
			mMeshGroup = DBG_NEW FTBasicMeshGroup;
		mMeshGroup->Initialize(meshData->GetMeshData(), mRenderer->GetDevice(), mRenderer->GetContext());

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

bool MeshRenderer::InitializeMesh(FTMeshData& meshData)
{
	if (!mMeshGroup)
		mMeshGroup = DBG_NEW FTBasicMeshGroup;

	std::vector<FTMeshData> meshes = { meshData };
	mMeshGroup->Initialize(meshes, mRenderer->GetDevice(), mRenderer->GetContext());
	if (!mMeshGroup)
	{
		LogString("ERROR: MeshRenderer::InitializeMesh() -> Mesh Init failed.\n");
		return false;
	}
	return true;
}

bool MeshRenderer::InitializeMesh(std::vector<FTMeshData>& meshData)
{
	if (!mMeshGroup)
		mMeshGroup = DBG_NEW FTBasicMeshGroup;

	mMeshGroup->Initialize(meshData, mRenderer->GetDevice(), mRenderer->GetContext());
	if (!mMeshGroup)
	{
		LogString("ERROR: MeshRenderer::InitializeMesh() -> Mesh Init failed.\n");
		return false;
	}
	return true;
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
			BasicVCData& vcd = mMeshGroup->GetVCData();
			vcd.model		 = modelMat.Transpose();
			vcd.view		 = viewMat.Transpose();
			vcd.projection	 = projMat.Transpose();
			vcd.invTranspose = std::move(invTransposeMat);

			mMeshGroup->UpdateConstantBuffers(mRenderer->GetDevice(), mRenderer->GetContext());
		}
	}
}

Matrix MeshRenderer::CalcModelMat(Transform* transform)
{
	int		  dir	= (int)transform->GetRightward().x;
	FTVector3 scale = transform->GetScale();

	DirectX::XMFLOAT3 scaleWithDir = DirectX::XMFLOAT3(scale.x, scale.y, scale.z);
	return Matrix::CreateScale(scaleWithDir) *
		Matrix::CreateRotationX(transform->GetRotation().x) *
		Matrix::CreateRotationY(transform->GetRotation().y) *
		Matrix::CreateRotationZ(transform->GetRotation().z) *
		Matrix::CreateTranslation(transform->GetWorldPosition().GetDXVec3());
}

MeshRenderer::MeshRenderer(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mMeshGroup(nullptr)
	, mRenderer(nullptr)
	, mMeshKey(ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	, mTexKey(ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	, mMaterialKeys()
{
}

MeshRenderer::~MeshRenderer()
{
	if (mMeshGroup)
	{
		delete mMeshGroup;
		mMeshGroup = nullptr;
	}
	mMaterialKeys.clear();
}

void MeshRenderer::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	// FileIOHelper::SaveBool(ofs, ChunkKey::FTMESHGROUP_DRAW_TEXTURE, mMeshGroup->GetDrawTexture());
	FileIOHelper::SaveBool(ofs, ChunkKey::FTMESHGROUP_DRAW_NORMALS, mMeshGroup->GetDrawNormal());

	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::MESH_KEY, mMeshKey);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::TEXTURE_KEY, mTexKey);
}

void MeshRenderer::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadUnsignedInt(ifs, mTexKey);
	FileIOHelper::LoadUnsignedInt(ifs, mMeshKey);

	bool drawVal = false;
	FileIOHelper::LoadBool(ifs, drawVal);
	mMeshGroup->SetDrawNormal(drawVal);

	FileIOHelper::LoadBool(ifs, drawVal);

	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void MeshRenderer::EditorRender(FoxtrotRenderer* renderer)
{
	if (mMeshGroup)
		UpdateMesh(GetOwner()->GetTransform(), EditorCamera::GetInstance());

	if (mMeshGroup)
	{
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		mMeshGroup->Render(renderer);
	}
}

void MeshRenderer::EditorUIUpdate()
{
	CHECK_RENDERER(GetRenderer());

	if (mMeshGroup)
	{
		mMeshGroup->UpdateUI();
		UINT key = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
		FTEditorUtils::DisplayResSelection(
			"Material",
			ResourceManager::GetInstance()->GetMapMaterials(),
			key);
		if (key != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
		{
			mMaterialKeys.push_back(key);
			mMeshGroup->SetMaterials(mMaterialKeys, mRenderer->GetDevice());
		}
		for (FTMaterial* mat : mMeshGroup->Materials())
			mat->UpdateUI();
	}

	if (ImGui::Button("Add Cube"))
		AddCube();
	if (ImGui::Button("Add Plane"))
		AddPlane();
	if (ImGui::Button("Add Cylinder"))
		AddCylinder();
	if (ImGui::Button("Add Sphere"))
		AddSphere();
	AddModel();

	UpdateSprite();
	OnConfirmUpdate();
}

void MeshRenderer::OnConfirmUpdate()
{
	if (ImGui::Button("UpdateSprite"))
		mMeshGroup->SetTexture(mTexKey);
}

void MeshRenderer::OnResetTexture()
{
	if (ImGui::Button("Reset"))
	{
		mMeshGroup->GetTexture()->ReleaseTexture();
		SetTexKey(ChunkKey::NullVal::VALUE_NOT_ASSIGNED);
	}
}

void MeshRenderer::UpdateSprite()
{
	std::string currentSprite = "No sprite has been assigned";
	if (mTexKey != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	{
		currentSprite =
			"Current sprite : \n" + ResourceManager::GetInstance()->GetLoadedTexture(GetTexKey())->GetRelativePath();
		if (mMeshGroup && mMeshGroup->GetTexture())
		{
			ImVec2 size = ImVec2(100, 100);
			ImGui::Image((ImTextureID)mMeshGroup->GetTexture()->GetResourceView().Get(), size);
		}
	}
	ImGui::Text(currentSprite.c_str());

	UINT key = mTexKey;
	FTEditorUtils::DisplayResSelection<FTTexture>(
		"Select Sprite",
		ResourceManager::GetInstance()->GetTexturesMap(),
		mTexKey);

	if (key != mTexKey)
		SetTexKey(mTexKey);
}

void MeshRenderer::UpdateSprite(UINT& key)
{
	std::string currentSprite = {};
	if (key != ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
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
			UINT	   spriteKey = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
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
void MeshRenderer::AddModel()
{
	UINT key = mMeshKey;
	FTEditorUtils::DisplayResSelection(
		"Select Mesh", ResourceManager::GetInstance()->GetMeshDataMap(), key);
	if (mMeshKey != key)
		InitializeMesh(key);
}

void MeshRenderer::AddCube()
{
	FTMeshData meshData =
		ResourceManager::GetInstance()->GetLoaded3DPrimitive(ChunkKey::PRIMITIVE_BOX);
	InitializeMesh(meshData);
	LogString("Cube added");
}

void MeshRenderer::AddPlane()
{
	FTMeshData meshData =
		ResourceManager::GetInstance()->GetLoaded3DPrimitive(ChunkKey::PRIMITIVE_SQUARE_GRID);
	InitializeMesh(meshData);
	LogString("Plane added");
}

void MeshRenderer::AddCylinder()
{
	FTMeshData meshData =
		ResourceManager::GetInstance()->GetLoaded3DPrimitive(ChunkKey::PRIMITIVE_CYLINDER);
	InitializeMesh(meshData);
	LogString("Cylinder added");
}

void MeshRenderer::AddSphere()
{
	FTMeshData meshData =
		ResourceManager::GetInstance()->GetLoaded3DPrimitive(ChunkKey::PRIMITIVE_SPHERE);
	InitializeMesh(meshData);
	LogString("Sphere added");
}
#endif // FOXTROT_EDITOR