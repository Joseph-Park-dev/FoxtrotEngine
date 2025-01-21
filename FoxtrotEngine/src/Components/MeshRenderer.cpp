// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/MeshRenderer.h"

#include <functional>

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
#endif // FOXTROT_EDITOR

using DXMatrix = DirectX::SimpleMath::Matrix;

void MeshRenderer::Initialize(FTCore* coreInstance)
{
	mRenderer = coreInstance->GetGameRenderer();
	if (mMeshKey != VALUE_NOT_ASSIGNED)
	{
		this->InitializeMesh();
		if (mTexKey != VALUE_NOT_ASSIGNED)
			SetTexture();
	}
	//mMeshGroup = DBG_NEW FTBasicMeshGroup;
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
		//renderer->SetRenderTargetView();
		mMeshGroup->Render(renderer, mTexture);
	}
}

void MeshRenderer::CloneTo(Actor* actor)
{
	MeshRenderer* newComp = DBG_NEW MeshRenderer(actor, GetUpdateOrder());
	newComp->mMeshKey = this->mMeshKey;
	newComp->mTexKey = this->mTexKey;
}

bool MeshRenderer::InitializeMesh()
{
	if (mMeshKey != VALUE_NOT_ASSIGNED) 
	{
		std::vector<MeshData>& meshData = ResourceManager::GetInstance()->GetLoadedMeshes(mMeshKey);
		if (!mMeshGroup)
			mMeshGroup = DBG_NEW FTBasicMeshGroup;
		mMeshGroup->Initialize(meshData, mRenderer->GetDevice(), mRenderer->GetContext());
		if (!mMeshGroup) {
			LogString("ERROR: MeshRenderer::InitializeMesh() -> Mesh Init failed.\n");
			return false;
		}
		return true;
	}
	else {
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
	if (mTexKey == VALUE_NOT_ASSIGNED)
	{
		printf("ERROR: MeshRenderer::SetTexture() -> TexKey not assigned.\n");
		return false;
	}
	mTexture = ResourceManager::GetInstance()->GetLoadedTexture(mTexKey);
	if (!mTexture)
		printf("ERROR: MeshRenderer::SetTexture() -> Cannot set texture %d, returning nullptr.\n", mTexKey);
	return mTexture != nullptr;
}

void MeshRenderer::UpdateMesh(Transform* transform, Camera* cameraInstance)
{
	if (mMeshGroup) 
	{
		UpdateConstantBufferModel(transform);
		UpdateConstantBufferView(cameraInstance);
		UpdateConstantBufferProjection(cameraInstance);
	}
}

void MeshRenderer::UpdateBuffers()
{
	if(mMeshGroup)
		mMeshGroup->UpdateConstantBuffers(mRenderer->GetDevice(), mRenderer->GetContext());
}

MeshRenderer::MeshRenderer(Actor* owner, int updateOrder)
	: Component		(owner, updateOrder)
	, mMeshGroup	(nullptr)
	, mTexture		(nullptr)
	, mRenderer		(nullptr)
	, mMeshKey		(VALUE_NOT_ASSIGNED)
	, mTexKey		(VALUE_NOT_ASSIGNED)
{}

MeshRenderer::~MeshRenderer()
{
	if (mMeshGroup)
	{
		delete mMeshGroup;
		mMeshGroup = nullptr;
	}
}

void MeshRenderer::UpdateConstantBufferModel(Transform* transform)
{
	for (Mesh* mesh : mMeshGroup->GetMeshes())
	{
		int dir = transform->GetRightward().x;
		FTVector3 worldPos = FTVector3(
			transform->GetWorldPosition().x,
			-transform->GetWorldPosition().y,
			transform->GetWorldPosition().z
		);
		FTVector3 scale = transform->GetScale();
		DirectX::XMFLOAT3 scaleWithDir = DirectX::XMFLOAT3(scale.x * dir, -scale.y, scale.z);
		Matrix model =
			DXMatrix::CreateScale(scaleWithDir) *
			DXMatrix::CreateRotationY(transform->GetRotation().y) *
			DXMatrix::CreateRotationX(transform->GetRotation().x) *
			DXMatrix::CreateRotationZ(transform->GetRotation().z) *
			DXMatrix::CreateTranslation(worldPos.GetDXVec3());
		mesh->VertexConstantData.model = model.Transpose();
	}
}

void MeshRenderer::UpdateConstantBufferView(Camera* camInst){
	for (Mesh* mesh : mMeshGroup->GetMeshes())
		mesh->VertexConstantData.view = camInst->GetViewRow().Transpose();
}

void MeshRenderer::UpdateConstantBufferProjection(Camera* camInst){
	for (Mesh* mesh : mMeshGroup->GetMeshes())
		mesh->VertexConstantData.projection = camInst->GetProjRow().Transpose();
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
void MeshRenderer::EditorUIUpdate()
{
	CHECK_RENDERER(GetRenderer());
	if (ImGui::Button("Add Cube"))
	{
		// mMeshGroup = ResourceManager::GetInstance()->GetLoadedMeshes("Box");
		LogString("Cube added");
	}
	OnConfirmUpdate();
}

void MeshRenderer::OnConfirmUpdate()
{
	if (ImGui::Button("Update"))
	{
		SetTexture();
	}
}
#endif // FOXTROT_EDITOR