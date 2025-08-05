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
	if (mMeshKey.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
	{
		this->InitializeMesh();
		if (mTexKey.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
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
		UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance(), renderer);
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		mMeshGroup->SetTexture();
		mMeshGroup->Render(renderer);
	}
}

void MeshRenderer::CloneTo(Actor* actor)
{
	MeshRenderer* newComp = DBG_NEW MeshRenderer(actor, GetUpdateOrder());
	newComp->mMeshKey.Assign(mMeshKey);
	newComp->mTexKey.Assign(mTexKey);
	for (size_t i = 0; i < mMaterialKeys.size(); ++i)
		newComp->mMaterialKeys.push_back(mMaterialKeys.at(i));
	newComp->mMeshGroup->SetDrawNormal(this->mMeshGroup->GetDrawNormal());
}

FoxtrotRenderer* MeshRenderer::GetRenderer() const { return mRenderer; }
FTDS::String&	 MeshRenderer::GetMeshKey() { return mMeshKey; }
FTDS::String&	 MeshRenderer::GetTexKey() { return mTexKey; }

FTBasicMeshGroup* MeshRenderer::GetMeshGroup() const { return mMeshGroup; }
FTTexture*		  MeshRenderer::GetTexture() const { return mMeshGroup->GetTexture(); }

void		  MeshRenderer::SetRenderer(FoxtrotRenderer* renderer) { mRenderer = renderer; }
void		  MeshRenderer::SetMeshKey(FTDS::String& key) { mMeshKey = key; }
void		  MeshRenderer::SetMeshKey(const char* key) { mMeshKey.Assign(key); }
void		  MeshRenderer::SetTexKey(FTDS::String& key) { mTexKey = key; }
void		  MeshRenderer::SetTexKey(const char* key) { mTexKey.Assign(key); }
FTDS::String& MeshRenderer::VSKey() { return mVSKey; }
FTDS::String& MeshRenderer::PSKey() { return mPSKey; }

void MeshRenderer::SetMeshGroup(FTBasicMeshGroup* meshGroup) 
{ 
	mMeshGroup = meshGroup; 
}

void MeshRenderer::SetMaterials()
{
	GetMeshGroup()->SetMaterials(mMaterialKeys, mRenderer->GetDevice());
}

std::vector<FTDS::String>& MeshRenderer::MaterialKeys() { return mMaterialKeys; }

bool MeshRenderer::InitializeMesh()
{
	if (mMeshKey.Equal(ChunkKey::NullVal::NULL_OBJECT))
	{
		LogString("ERROR: MeshRenderer::InitializeMesh() -> Key doesn't exist.\n");
		return false;
	}

#ifdef FOXTROT_EDITOR
	mMeshGroup = EditorResourceManager::GetInstance()->GetLoadedMesh(mMeshKey);
#else
	mMeshGroup = ResourceManager::GetInstance()->GetLoadedMesh(mMeshKey);
#endif // FOXTROT_EDITOR

	if (!mMeshGroup)
	{
		Debug::LogError(__LINE__, __FILE__, "MeshGroup cannot be found");
		return false;
	}
	return true;
}

bool MeshRenderer::InitializeMesh(FTDS::String& key)
{
	mMeshKey   = key;
	return InitializeMesh();
}

void MeshRenderer::UpdateMesh(Transform* transform, Camera* camInst, FoxtrotRenderer* renderer)
{
	if (mMeshGroup)
	{
		mMeshGroup->CalcVCData(transform, camInst);
		mMeshGroup->UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());
	}
}

// void MeshRenderer::TEST_UpdateMesh(Transform* transform, Camera* camInst)
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
// }

// Matrix MeshRenderer::TEST_CalcModelMat(Transform* transform)
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
// }

MeshRenderer::MeshRenderer(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
	, mMeshGroup(nullptr)
	, mRenderer(nullptr)
	, mMeshKey(ChunkKey::NullVal::NULL_OBJECT)
	, mTexKey(ChunkKey::NullVal::NULL_OBJECT)
	, mVSKey(ChunkKey::NullVal::NULL_OBJECT)
	, mPSKey(ChunkKey::NullVal::NULL_OBJECT)
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

	FileIOHelper::SaveString(ofs, ChunkKey::MESH_KEY, mMeshKey.C_Str());
	FileIOHelper::SaveString(ofs, ChunkKey::TEXTURE_KEY, mTexKey.C_Str());

	// Save FTMaterial keys
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::MATERIAL_KEYS);

	for (size_t i = 0; i < mMaterialKeys.size(); ++i)
		FileIOHelper::SaveString(ofs, std::to_string(i).c_str(), mMaterialKeys.at(i));
	FileIOHelper::SaveSize(ofs, ChunkKey::MATERIAL_COUNT, mMaterialKeys.size());

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::MATERIAL_KEYS);

	// Save Shader keys.
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::SHADER_KEYS);

	FileIOHelper::SaveString(ofs, ChunkKey::FT_VERTEX_SHADER, mVSKey);
	FileIOHelper::SaveString(ofs, ChunkKey::FT_PIXEL_SHADER, mPSKey);

	FileIOHelper::EndDataPackSave(ofs, ChunkKey::SHADER_KEYS);
}

void MeshRenderer::LoadProperties(std::ifstream& ifs)
{
	// Load Shader keys.
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::SHADER_KEYS);

	FileIOHelper::LoadBasicString(ifs, mPSKey);
	FileIOHelper::LoadBasicString(ifs, mVSKey);

	// Save FTMaterial keys
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::MATERIAL_KEYS);

	size_t matCount = 0;
	FileIOHelper::LoadSize(ifs, matCount);
	for (size_t i = 0; i < matCount; ++i)
	{
		FTDS::String key = ChunkKey::NullVal::NULL_OBJECT;
		FileIOHelper::LoadBasicString(ifs, key);
		mMaterialKeys.push_back(key);
	}

	FileIOHelper::LoadBasicString(ifs, mTexKey);
	FileIOHelper::LoadBasicString(ifs, mMeshKey);

	bool drawVal = false;
	FileIOHelper::LoadBool(ifs, drawVal);

	Component::LoadProperties(ifs);

	InitializeMesh(mMeshKey);
	if (mMeshGroup)
	{
		mMeshGroup->SetDrawNormal(drawVal);
		mMeshGroup->SetTexture(mTexKey);
		mMeshGroup->SetVertexShader(mVSKey);
		mMeshGroup->SetPixelShader(mPSKey);
	}
}

#ifdef FOXTROT_EDITOR
void MeshRenderer::EditorRender(FoxtrotRenderer* renderer)
{
	if (mMeshGroup)
	{
		UpdateMesh(GetOwner()->GetTransform(), EditorCamera::GetInstance(), renderer);
		renderer->SwitchFillMode();
		// renderer->SetRenderTargetView();
		mMeshGroup->Render(renderer);
	}

	// if (mMeshGroup)
	//	TEST_UpdateMesh(GetOwner()->GetTransform(), EditorCamera::GetInstance());

	// if (mMeshGroup)
	//{
	//	renderer->SwitchFillMode();
	//	// renderer->SetRenderTargetView();
	//	TEST_MESH->Render(renderer);
	// }
}

void MeshRenderer::EditorUIUpdate()
{
	CHECK_RENDERER(GetRenderer());

	if (!mMeshGroup)
		return;

	ImGui::SeparatorText("Material");
	mMeshGroup->UpdateUI();
	UpdateMaterial();

	ImGui::SeparatorText("Shaders");
	UpdateVS();
	UpdatePS();
	AddModel();

	UpdateSprite();
}

void MeshRenderer::OnResetTexture()
{
	if (ImGui::Button("Reset"))
	{
		mMeshGroup->GetTexture()->ReleaseTexture();
		SetTexKey(ChunkKey::NullVal::NULL_OBJECT);
	}
}

void MeshRenderer::UpdateSprite()
{
	FTDS::String currentSprite = "No sprite has been assigned";
	if (mTexKey.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
	{
		currentSprite =
			FTDS::String("Current sprite : \n") + EditorResourceManager::GetInstance()->GetLoadedTexture(mTexKey)->RelativePath().C_Str();
		if (mMeshGroup && mMeshGroup->GetTexture())
		{
			ImVec2 size = ImVec2(100, 100);
			ImGui::Image((ImTextureID)mMeshGroup->GetTexture()->GetResourceView().Get(), size);
		}
	}
	ImGui::Text(currentSprite.C_Str());

	FTDS::String key = mTexKey;
	FTEditorUtils::DisplayResSelection<FTTexture>(
		"Select Sprite",
		EditorResourceManager::GetInstance()->GetTextures(),
		key);

	if (key.NotEqual(mTexKey.C_Str()))
	{
		SetTexKey(key);
		if (mMeshGroup)
			mMeshGroup->SetTexture(mTexKey);
	}
}

void MeshRenderer::UpdateSprite(FTDS::String& key)
{
	FTDS::String currentSprite = {};
	if (!key.Equal(ChunkKey::NullVal::NULL_OBJECT))
	{
		FTTexture* sprite = EditorResourceManager::GetInstance()->GetLoadedTexture(key);
		currentSprite =
			FTDS::String("Current sprite : \n") + sprite->RelativePath().C_Str();
		if (sprite)
		{
			ImVec2 size = ImVec2(100, 100);
			ImGui::Image((ImTextureID)sprite->GetResourceView().Get(), size);
		}
	}
	else
		currentSprite = "No sprite has been assigned";

	ImGui::Text(currentSprite.C_Str());

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
		FTDS::HashMap<FTTexture*>* texturesMap =
			EditorResourceManager::GetInstance()->GetTextures();
		if (ImGui::TreeNode("Selection State: Single Selection"))
		{
			FTDS::String spriteKey = ChunkKey::NullVal::NULL_OBJECT;
			static int	 selected  = -1;
			int			 i		   = 0;
			for (auto iter = texturesMap->Begin(); iter != texturesMap->End();
				 ++iter, ++i)
			{
				if ((*iter)->Value())
				{
					if (ImGui::Selectable((*iter)->Value()->FileName().C_Str(), selected == i))
					{
						spriteKey = (*iter)->Value()->FileName();
						selected  = i;
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
	// Select & load Materials.
	FTDS::String key = ChunkKey::NullVal::NULL_OBJECT;
	FTEditorUtils::DisplayResSelection(
		"Material",
		EditorResourceManager::GetInstance()->GetMaterials(),
		key);
	if (key.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
	{
		mMaterialKeys.push_back(key);
		if (mMeshGroup)
			mMeshGroup->SetMaterials(mMaterialKeys, mRenderer->GetDevice());
	}

	// Display loaded Materials.
	if (!mMeshGroup->Materials().empty())
	{
		for (FTMaterial* mat : mMeshGroup->Materials())
			mat->UpdateUI();
	}
}

void MeshRenderer::UpdateVS()
{
	FTDS::String vsKey = mVSKey;
	FTEditorUtils::DisplayResSelection("Vertex Shader", EditorResourceManager::GetInstance()->GetVertexShaders(), vsKey);

	if (mVSKey.NotEqual(vsKey.C_Str()))
	{
		mVSKey.Assign(vsKey);
		GetMeshGroup()->SetVertexShader(mVSKey);
	}
}

void MeshRenderer::UpdatePS()
{
	FTDS::String psKey = mPSKey;
	FTEditorUtils::DisplayResSelection("Pixel Shader", EditorResourceManager::GetInstance()->GetPixelShaders(), psKey);

	if (mPSKey.NotEqual(psKey.C_Str()))
	{
		mPSKey.Assign(psKey);
		GetMeshGroup()->SetPixelShader(mPSKey);
	}
}

void MeshRenderer::AddModel()
{
	FTDS::String key = mMeshKey;
	FTEditorUtils::DisplayResSelection(
		"Select Mesh", EditorResourceManager::GetInstance()->GetMeshGroups(), key);
	if (mMeshKey != key)
		InitializeMesh(key);
}
#endif // FOXTROT_EDITOR