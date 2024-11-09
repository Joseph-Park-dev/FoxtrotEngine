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

#ifdef FOXTROT_EDITOR
#include "FTCoreEditor.h"
#endif // FOXTROT_EDITOR

using DXMatrix = DirectX::SimpleMath::Matrix;

void MeshRendererComponent::Initialize(FTCore* coreInstance){
	mRenderer = coreInstance->GetGameRenderer();
	Component::Initialize(coreInstance);
}

void MeshRendererComponent::Update(float deltaTime){
	if (mMeshGroup) {
		UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance());
		UpdateBuffers();
	}
}

void MeshRendererComponent::Render(FoxtrotRenderer* renderer){
	if (mMeshGroup){
		renderer->SwitchFillMode();
		mMeshGroup->Render(renderer->GetContext());
	}
}

bool MeshRendererComponent::InitializeMesh(){
	if (mMeshKey != VALUE_NOT_ASSIGNED) {
		std::vector<MeshData>& meshData = ResourceManager::GetInstance()->GetLoadedMeshes(mMeshKey);
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

bool MeshRendererComponent::InitializeMesh(MeshData meshData)
{
	std::vector<MeshData> meshes = { meshData };
	mMeshGroup->Initialize(meshes, mRenderer->GetDevice(), mRenderer->GetContext());
	if (!mMeshGroup) {
		LogString("ERROR: MeshRendererComponent::InitializeMesh() -> Mesh Init failed.\n");
		return false;
	}
	return true;
}

void MeshRendererComponent::UpdateMesh(Transform* transform, Camera* cameraInstance){
	if (mMeshGroup) {
		UpdateConstantBufferModel(transform);
		UpdateConstantBufferView(cameraInstance);
		UpdateConstantBufferProjection(cameraInstance);
	}
}

void MeshRendererComponent::UpdateBuffers(){
	if (GetMeshGroup()->GetVertexConstantBuffer().Get()) {
		D3D11Utils::UpdateBuffer(
			mRenderer->GetDevice(),
			mRenderer->GetContext(),
			GetMeshGroup()->GetVertexConstantData(),
			GetMeshGroup()->GetVertexConstantBuffer()
		);
	}

	if (GetMeshGroup()->GetPixelConstantBuffer().Get()) {
		D3D11Utils::UpdateBuffer(
			mRenderer->GetDevice(),
			mRenderer->GetContext(),
			GetMeshGroup()->GetPixelConstantData(),
			GetMeshGroup()->GetPixelConstantBuffer()
		);
	}
}

void MeshRendererComponent::SetTexture() {
	if (mTexKey != VALUE_NOT_ASSIGNED)
		GetMeshGroup()->SetTexture(mTexKey);
	else 
		printf("ERROR: MeshRendererComponent::SetTexture() -> key is not assigned.\n", mTexKey);
}

MeshRendererComponent::MeshRendererComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder, updateOrder)
	, mMeshGroup(new FTBasicMeshGroup)
	, mRenderer(nullptr)
	, mMeshKey(VALUE_NOT_ASSIGNED)
	, mTexKey(VALUE_NOT_ASSIGNED)
{}

MeshRendererComponent::~MeshRendererComponent(){
	if (mMeshGroup)
		delete mMeshGroup;
}

void MeshRendererComponent::UpdateConstantBufferModel(Transform* transform){
	FTVector3 scale = transform->GetScale();
	int dir = transform->GetCurrentDirection();
	DirectX::XMFLOAT3 scaleWithDir = DirectX::XMFLOAT3(scale.x * dir, scale.y, scale.z);
	FTVector3 worldPos = FTVector3(
		transform->GetWorldPosition().x * Camera::GetInstance()->GetNDCRatio(),
		transform->GetWorldPosition().y * Camera::GetInstance()->GetNDCRatio(),
		transform->GetWorldPosition().z
	);
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
void MeshRendererComponent::SaveProperties(nlohmann::ordered_json& out)
{
	Component::SaveProperties(out);
	FileIOHelper::AddScalarValue(out["MeshKey"], mMeshKey);
	FileIOHelper::AddScalarValue(out["TextureKey"], mTexKey);
}

void MeshRendererComponent::LoadProperties(nlohmann::ordered_json& in)
{
}

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
#endif