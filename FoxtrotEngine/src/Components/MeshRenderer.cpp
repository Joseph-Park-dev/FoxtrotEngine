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

	if (0 < mMaterialKeys.size())
		mMeshGroup->SetMaterials(mMaterialKeys, mRenderer->GetDevice());

	Component::Initialize(coreInstance);
}

void MeshRenderer::Render(FoxtrotRenderer* renderer)
{
	if (mMeshGroup)
	{
		UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance());
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
	for (size_t i = 0; i < mMaterialKeys.size(); ++i)
		newComp->mMaterialKeys.push_back(mMaterialKeys.at(i));
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

void MeshRenderer::SetMaterials()
{
	GetMeshGroup()->SetMaterials(mMaterialKeys, mRenderer->GetDevice());
}

std::vector<UINT>& MeshRenderer::MaterialKeys() { return mMaterialKeys; }

bool MeshRenderer::InitializeMesh()
{
	if (mMeshKey == ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	{
		LogString("ERROR: MeshRenderer::InitializeMesh() -> Key doesn't exist.\n");
		return false;
	}
	mMeshGroup = ResourceManager::GetInstance()->GetLoadedMesh(mMeshKey);
	if (!mMeshGroup)
	{
		Debug::LogError(__LINE__, __FILE__, "MeshGroup cannot be found");
		return false;
	}
	return true;
}

bool MeshRenderer::InitializeMesh(UINT key)
{
	mMeshKey = key;
	mMeshGroup = ResourceManager::GetInstance()->GetLoadedMesh(key);
	return mMeshGroup != nullptr;
}

void MeshRenderer::UpdateMesh(Transform* transform, Camera* camInst)
{
	if (mMeshGroup)
	{
		mMeshGroup->CalcVCData(transform, camInst);
		mMeshGroup->UpdateConstantBuffers(mRenderer->GetDevice(), mRenderer->GetContext());
	}
}

//void MeshRenderer::TEST_UpdateMesh(Transform* transform, Camera* camInst)
//{
//	if (TEST_MESH)
//	{
//		// Model Transformation
//		Matrix&& modelMat = std::move(TEST_CalcModelMat(transform));
//		Matrix	 invTransposeMat = modelMat.Transpose();
//		invTransposeMat.Translation(Vector3(0.0f));
//		invTransposeMat = invTransposeMat.Transpose().Invert();
//
//		// View Transformation
//		Matrix&& viewMat = camInst->GetViewRow();
//		Vector3	 eyeWorld = Vector3::Transform(Vector3(0.0f), viewMat.Invert());
//
//		// Project Transformation
//		Matrix&& projMat = std::move(camInst->GetProjRow());
//
//		for (Mesh* mesh : TEST_MESH->Meshes())
//		{
//			BasicVCData& vcd = TEST_MESH->GetVCData();
//			vcd.model = modelMat.Transpose();
//			vcd.view = viewMat.Transpose();
//			vcd.projection = projMat.Transpose();
//			vcd.invTranspose = std::move(invTransposeMat);
//
//			TEST_MESH->UpdateConstantBuffers(mRenderer->GetDevice(), mRenderer->GetContext());
//		}
//	}
//}

//Matrix MeshRenderer::TEST_CalcModelMat(Transform* transform)
//{
//	int		  dir = (int)transform->GetRightward().x;
//	FTVector3 scale = transform->GetScale();
//
//	DirectX::XMFLOAT3 scaleWithDir = DirectX::XMFLOAT3(scale.x, scale.y, scale.z);
//	return Matrix::CreateScale(scaleWithDir) *
//		Matrix::CreateRotationX(transform->GetRotation().x) *
//		Matrix::CreateRotationY(transform->GetRotation().y) *
//		Matrix::CreateRotationZ(transform->GetRotation().z) *
//		Matrix::CreateTranslation(FTVector3::Zero.GetDXVec3());
//}

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
		mMeshGroup = nullptr;
	mMaterialKeys.clear();
}

void MeshRenderer::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	// FileIOHelper::SaveBool(ofs, ChunkKey::FTMESHGROUP_DRAW_TEXTURE, mMeshGroup->GetDrawTexture());
	FileIOHelper::SaveBool(ofs, ChunkKey::FTMeshGroup::DRAW_NORMALS, mMeshGroup->GetDrawNormal());

	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::MESH_KEY, mMeshKey);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::TEXTURE_KEY, mTexKey);

	// Save FTMaterial keys
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::MATERIAL_KEYS);

	for (size_t i = 0; i < mMaterialKeys.size(); ++i)
		FileIOHelper::SaveUnsignedInt(ofs, std::to_string(i), mMaterialKeys.at(i));
	FileIOHelper::SaveSize(ofs, ChunkKey::MATERIAL_COUNT, mMaterialKeys.size());

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::MATERIAL_KEYS);
}

void MeshRenderer::LoadProperties(std::ifstream& ifs)
{
	// Save FTMaterial keys
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::MATERIAL_KEYS);

	size_t matCount = 0;
	FileIOHelper::LoadSize(ifs, matCount);
	for (size_t i = 0; i < matCount; ++i)
	{
		UINT key = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
		FileIOHelper::LoadUnsignedInt(ifs, key);
		mMaterialKeys.push_back(key);
	}

	FileIOHelper::LoadUnsignedInt(ifs, mTexKey);
	FileIOHelper::LoadUnsignedInt(ifs, mMeshKey);

	bool drawVal = false;
	FileIOHelper::LoadBool(ifs, drawVal);

	Component::LoadProperties(ifs);

	InitializeMesh(mMeshKey);
	if (mMeshGroup)
	{
		mMeshGroup->SetDrawNormal(drawVal);
		mMeshGroup->SetTexture(mTexKey);
	}
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

	//if (mMeshGroup)
	//	TEST_UpdateMesh(GetOwner()->GetTransform(), EditorCamera::GetInstance());

	//if (mMeshGroup)
	//{
	//	renderer->SwitchFillMode();
	//	// renderer->SetRenderTargetView();
	//	TEST_MESH->Render(renderer);
	//}
}

void MeshRenderer::EditorUIUpdate()
{
	CHECK_RENDERER(GetRenderer());

	if (mMeshGroup)
	{
		mMeshGroup->UpdateUI();
		UpdateMaterial();
	}
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
	{
		SetTexKey(mTexKey);
		mMeshGroup->SetTexture(mTexKey);
	}
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
				if ((*iter).second)
				{
					if (ImGui::Selectable((*iter).second->GetFileName().c_str(), selected == i))
					{
						spriteKey = (*iter).first;
						selected = i;
					}
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
void MeshRenderer::UpdateMaterial()
{
	// Display loaded Materials.
	if (0 < mMaterialKeys.size())
	{
		for (UINT key : mMaterialKeys)
			ResourceManager::GetInstance()->GetMapMaterials().at(key)->UpdateUI();
	}
	else
		ImGui::Text("No Material has been assigned");

	// Select & load Materials.
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
}

void MeshRenderer::AddModel()
{
	UINT key = mMeshKey;
	FTEditorUtils::DisplayResSelection(
		"Select Mesh", ResourceManager::GetInstance()->GetMeshGroupsMap(), key);
	if (mMeshKey != key)
		InitializeMesh(key);
}
#endif // FOXTROT_EDITOR