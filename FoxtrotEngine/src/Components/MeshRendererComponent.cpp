// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Components/MeshRendererComponent.h"

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

void MeshRendererComponent::Initialize(FTCore* coreInstance)
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

void MeshRendererComponent::Update(float deltaTime)
{
	if (mMeshGroup) 
	{
		UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance());
		UpdateBuffers();
	}
}

void MeshRendererComponent::Render(FoxtrotRenderer* renderer)
{
	if (mMeshGroup)
	{
		renderer->SwitchFillMode();
		//renderer->SetRenderTargetView();
		mMeshGroup->Render(renderer, mTexture);
	}
}

bool MeshRendererComponent::InitializeMesh()
{
	if (mMeshKey != VALUE_NOT_ASSIGNED) 
	{
		std::vector<MeshData>& meshData = ResourceManager::GetInstance()->GetLoadedMeshes(mMeshKey);
		if (!mMeshGroup)
			mMeshGroup = DBG_NEW FTBasicMeshGroup;
		mMeshGroup->Initialize(meshData, mRenderer->GetDevice(), mRenderer->GetContext());
		if (!mMeshGroup) {
			LogString("ERROR: MeshRendererComponent::InitializeMesh() -> Mesh Init failed.\n");
			return false;
		}
		return true;
	}
	else {
		LogString("ERROR: MeshRendererComponent::InitializeMesh() -> Key doesn't exist.\n");
		return false;
	}
}

bool MeshRendererComponent::InitializeMesh(UINT key)
{
	mMeshKey = key;
	MeshRendererComponent::InitializeMesh();
	return mMeshGroup != nullptr;
}

bool MeshRendererComponent::InitializeMesh(MeshData& meshData)
{
	if (!mMeshGroup)
		mMeshGroup = DBG_NEW FTBasicMeshGroup;
	std::vector<MeshData> meshes = { meshData };
	mMeshGroup->Initialize(meshes, mRenderer->GetDevice(), mRenderer->GetContext());
	if (!mMeshGroup) 
	{
		LogString("ERROR: MeshRendererComponent::InitializeMesh() -> Mesh Init failed.\n");
		return false;
	}
	return true;
}

bool MeshRendererComponent::SetTexture()
{
	if (mTexKey == VALUE_NOT_ASSIGNED)
	{
		printf("ERROR: MeshRendererComponent::SetTexture() -> TexKey not assigned.\n");
		return false;
	}
	mTexture = ResourceManager::GetInstance()->GetLoadedTexture(mTexKey);
	if (!mTexture)
		printf("ERROR: MeshRendererComponent::SetTexture() -> Cannot set texture %d, returning nullptr.\n", mTexKey);
	return mTexture != nullptr;
}

void MeshRendererComponent::UpdateMesh(Transform* transform, Camera* cameraInstance)
{
	if (mMeshGroup) 
	{
		UpdateConstantBufferModel(transform);
		UpdateConstantBufferView(cameraInstance);
		UpdateConstantBufferProjection(cameraInstance);
	}
}

void MeshRendererComponent::UpdateBuffers()
{
	if(mMeshGroup)
		mMeshGroup->UpdateConstantBuffers(mRenderer->GetDevice(), mRenderer->GetContext());
}

MeshRendererComponent::MeshRendererComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component		(owner, drawOrder, updateOrder)
	, mMeshGroup	(nullptr)
	, mTexture		(nullptr)
	, mRenderer		(nullptr)
	, mMeshKey		(VALUE_NOT_ASSIGNED)
	, mTexKey		(VALUE_NOT_ASSIGNED)
{}

MeshRendererComponent::~MeshRendererComponent()
{
	if (mMeshGroup)
	{
		delete mMeshGroup;
		mMeshGroup = nullptr;
	}
}

void MeshRendererComponent::UpdateConstantBufferModel(Transform* transform)
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
	mMeshGroup->GetVertexConstantData().model = model.Transpose();
}

void MeshRendererComponent::UpdateConstantBufferView(Camera* camInst){
	mMeshGroup->GetVertexConstantData().view = camInst->GetViewRow().Transpose();
}

void MeshRendererComponent::UpdateConstantBufferProjection(Camera* camInst){
	mMeshGroup->GetVertexConstantData().projection = camInst->GetProjRow().Transpose();
}

void MeshRendererComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::MESH_KEY, mMeshKey);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKeys::TEXTURE_KEY, mTexKey);
}

void MeshRendererComponent::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadUnsignedInt(ifs, mTexKey);
	FileIOHelper::LoadUnsignedInt(ifs, mMeshKey);
	Component::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void MeshRendererComponent::EditorUIUpdate()
{
	CHECK_RENDERER(GetRenderer());
	if (ImGui::Button("Add Cube"))
	{
		// mMeshGroup = ResourceManager::GetInstance()->GetLoadedMeshes("Box");
		LogString("Cube added");
	}
	OnConfirmUpdate();
}

void MeshRendererComponent::OnConfirmUpdate()
{
	if (ImGui::Button("Update"))
	{
		SetTexture();
	}
}
void MeshRendererComponent::CloneTo(Actor* actor)
{
	MeshRendererComponent* newComp = DBG_NEW MeshRendererComponent(actor, GetDrawOrder(), GetUpdateOrder());
	newComp->mMeshKey = this->mMeshKey;
	newComp->mTexKey = this->mTexKey;
}
#endif // FOXTROT_EDITOR