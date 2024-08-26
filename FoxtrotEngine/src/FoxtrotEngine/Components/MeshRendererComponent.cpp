#include "FoxtrotEngine/Components/MeshRendererComponent.h"

#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/ResourceSystem/Mesh.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"
#include "FoxtrotEngine/ResourceSystem/FTBasicMeshGroup.h"
#include "FoxtrotEngine/Renderer/Camera.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/ResourceSystem/GeometryGenerator.h"
#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/FileSystem/ChunkLoader.h"

#ifdef _DEBUG
#include "FoxtrotEditor/FTCoreEditor.h"
#endif

using DXMatrix = DirectX::SimpleMath::Matrix;

void MeshRendererComponent::Initialize(FTCore* coreInstance){
	mRenderer = coreInstance->GetGameRenderer();
}

void MeshRendererComponent::Update(float deltaTime){
	if (mMeshGroup) {
		UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance());
		UpdateBuffers(mRenderer);
	}
}

void MeshRendererComponent::Render(FoxtrotRenderer* renderer){
	if (mMeshGroup){
		mMeshGroup->Render(renderer->GetContext());
	}
}

void MeshRendererComponent::InitializeMesh(){
	mMeshGroup = ResourceManager::GetInstance()->GetLoadedMeshes(mKey);
}

void MeshRendererComponent::UpdateMesh(Transform* transform, Camera* cameraInstance){
	if (mMeshGroup) {
		UpdateConstantBufferModel(transform);
		UpdateConstantBufferView(cameraInstance);
		UpdateConstantBufferProjection(cameraInstance);
	}
}

void MeshRendererComponent::UpdateBuffers(FoxtrotRenderer* renderer){
	mMeshGroup->UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());
}

void MeshRendererComponent::SetTexture(Mesh* mesh, FTTexture* texture) {
	if(mesh)
		mesh->texture = texture;
}

MeshRendererComponent::MeshRendererComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component	(owner, drawOrder, updateOrder)
	, mRenderer	 (nullptr)
	, mMeshGroup (nullptr)
{}

MeshRendererComponent::~MeshRendererComponent(){
	if (mMeshGroup)
		delete mMeshGroup;
}

void MeshRendererComponent::UpdateConstantBufferModel(Transform* transform){
	DirectX::XMFLOAT3 scale = transform->GetScale().GetDXVec3();
	DirectX::XMFLOAT3 worldPos = transform->GetWorldPosition().GetDXVec3();
	// 모델의 변환 -> 모델 행렬 결정
	mMeshGroup->GetVertexConstantData().model =
		DXMatrix::CreateScale(scale) *
		DXMatrix::CreateRotationY(transform->GetRotation().y) *
		DXMatrix::CreateRotationX(transform->GetRotation().x) *
		DXMatrix::CreateRotationZ(transform->GetRotation().z) *
		DXMatrix::CreateTranslation(worldPos);
	mMeshGroup->GetVertexConstantData().model = mMeshGroup->GetVertexConstantData().model.Transpose();
}

void MeshRendererComponent::UpdateConstantBufferView(Camera* camInst){
	DirectX::XMFLOAT3 eyePosDX = camInst->GetViewEyePosDX();
	DirectX::XMFLOAT3 eyeDir = camInst->GetViewEyeDirDX();
	DirectX::XMFLOAT3 up = camInst->GetViewUpDX();

	DirectX::XMVECTOR viewEyePos = DirectX::XMLoadFloat3(&eyePosDX);
	DirectX::XMVECTOR viewEyeDir = DirectX::XMLoadFloat3(&eyeDir);
	DirectX::XMVECTOR viewUp = DirectX::XMLoadFloat3(&up);

	FTVector3 viewEyeRotation = camInst->GetViewEyeRotation();
	
	mMeshGroup->GetVertexConstantData().view =
		DirectX::XMMatrixLookToLH(viewEyePos, viewEyeDir, viewUp);

	mMeshGroup->GetVertexConstantData().view *=
		DXMatrix::CreateRotationY(viewEyeRotation.y) *
		DXMatrix::CreateRotationX(viewEyeRotation.x) *
		DXMatrix::CreateRotationZ(viewEyeRotation.z);
		
	//mesh.basicVertexConstantBufferData.view = 
	//	DirectX::SimpleMath::Matrix::CreateScale(Camera::GetInstance()->GetZoomValue());
	mMeshGroup->GetVertexConstantData().view = mMeshGroup->GetVertexConstantData().view.Transpose();
}

void MeshRendererComponent::UpdateConstantBufferProjection(Camera* camInst){
	float projFOVAngleY = DirectX::XMConvertToRadians(Camera::GetInstance()->GetProjFOVAngleY());
	float aspect = camInst->GetAspect();
	float nearZ = camInst->GetNearZ();
	float farZ = camInst->GetFarZ();
	if (mRenderer->GetViewType() == Viewtype::Perspective) {
		mMeshGroup->GetVertexConstantData().projection =
			DirectX::XMMatrixPerspectiveFovLH(
				projFOVAngleY, aspect, nearZ, farZ
			);
	}
	else if (mRenderer->GetViewType() == Viewtype::Orthographic){
		mMeshGroup->GetVertexConstantData().projection =
			DirectX::XMMatrixOrthographicOffCenterLH(
				-aspect, aspect, -1.0f, 1.0f, nearZ, farZ
			);
	}
	mMeshGroup->GetVertexConstantData().projection =
		mMeshGroup->GetVertexConstantData().projection.Transpose();
}

void MeshRendererComponent::SaveProperties(nlohmann::ordered_json& out)
{
	Component::SaveProperties(out);
	FileIOHelper::AddScalarValue(out["Key"], mKey);
}

void MeshRendererComponent::EditorUpdate(float deltaTime){
	Update(deltaTime);
}

void MeshRendererComponent::EditorRender(FoxtrotRenderer* renderer)
{

}

void MeshRendererComponent::EditorUIUpdate(){
	if (ImGui::Button("Add Cube")) {
		mMeshGroup = ResourceManager::GetInstance()->GetLoadedMeshes("Box");
	}
}