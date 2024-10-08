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

#ifdef FOXTROT_EDITOR
#include "FTCoreEditor.h"
#endif // FOXTROT_EDITOR

using DXMatrix = DirectX::SimpleMath::Matrix;

void MeshRendererComponent::Initialize(FTCore* coreInstance){
	mRenderer = coreInstance->GetGameRenderer();
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
		LogString("ERROR: MeshRendererComponent::SetTexture() -> Key doesn't exist.\n");
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
	DirectX::XMFLOAT3 worldPos = transform->GetWorldPosition().GetDXVec3();
	// 모델의 변환 -> 모델 행렬 결정
	Matrix model =
		DXMatrix::CreateScale(scaleWithDir) *
		DXMatrix::CreateRotationY(transform->GetRotation().y) *
		DXMatrix::CreateRotationX(transform->GetRotation().x) *
		DXMatrix::CreateRotationZ(transform->GetRotation().z) *
		DXMatrix::CreateTranslation(worldPos);
	mMeshGroup->GetVertexConstantData().model = model.Transpose();
}

void MeshRendererComponent::UpdateConstantBufferView(Camera* camInst){
	mMeshGroup->GetVertexConstantData().view = camInst->GetViewRow().Transpose();
}

void MeshRendererComponent::UpdateConstantBufferProjection(Camera* camInst){
	mMeshGroup->GetVertexConstantData().projection = camInst->GetProjRow().Transpose();
}

#ifdef FOXTROT_EDITOR
void MeshRendererComponent::SaveProperties(nlohmann::ordered_json& out)
{
	Component::SaveProperties(out);
	FileIOHelper::AddScalarValue(out["MeshKey"], mMeshKey);
	FileIOHelper::AddScalarValue(out["TextureKey"], mTexKey);
}

void MeshRendererComponent::EditorUpdate(float deltaTime)
{
	Update(deltaTime);
}

void MeshRendererComponent::EditorRender(FoxtrotRenderer* renderer)
{
	Render(renderer);
}

void MeshRendererComponent::EditorUIUpdate()
{
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