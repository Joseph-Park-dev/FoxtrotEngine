#include "FoxtrotEngine/Components/MeshRendererComponent.h"

#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/ResourceSystem/Mesh.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"
#include "FoxtrotEngine/Renderer/Camera.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"

#ifdef _DEBUG
#include "FoxtrotEditor/FTCoreEditor.h"
#endif

void MeshRendererComponent::InitializeMesh(FTCore* coreInstance, MeshData&& meshData)
{
	mRenderer = coreInstance->GetGameRenderer();
	mRenderer->CreateVertexBuffer(meshData.vertices, mMesh->vertexBuffer);
	mMesh->indexCount = UINT(meshData.indices.size());
	mRenderer->CreateIndexBuffer(meshData.indices, mMesh->indexBuffer);

	// Create Constant buffers
	mMesh->basicVertexConstantBufferData.model = DirectX::SimpleMath::Matrix();
	mMesh->basicVertexConstantBufferData.view = DirectX::SimpleMath::Matrix();
	mMesh->basicVertexConstantBufferData.projection = DirectX::SimpleMath::Matrix();
	mRenderer->CreateConstantBuffer(mMesh->basicVertexConstantBufferData,
		mMesh->vertexConstantBuffer);
	mRenderer->CreateConstantBuffer(mMesh->pixelShaderConstantBufferData,
		mMesh->pixelConstantBuffer);
}

void MeshRendererComponent::UpdateMesh(Transform* transform, Camera* cameraInstance)
{
	UpdateConstantBufferModel(mMesh, transform);
	UpdateConstantBufferView(mMesh, cameraInstance);
	UpdateConstantBufferProjection(mMesh, cameraInstance);
}

void MeshRendererComponent::RenderMesh(FoxtrotRenderer* renderer)
{
	if (mMesh) {
		renderer->UpdateBuffer(mMesh->basicVertexConstantBufferData,
			mMesh->vertexConstantBuffer);
		renderer->UpdateBuffer(mMesh->pixelShaderConstantBufferData,
			mMesh->pixelConstantBuffer);
	}
}

MeshRendererComponent::MeshRendererComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component	(owner, drawOrder, updateOrder)
	, mRenderer	(nullptr)
	, mMesh		(new Mesh)
{}

MeshRendererComponent::~MeshRendererComponent()
{}

void MeshRendererComponent::UpdateConstantBufferModel(Mesh* mesh, Transform* transform)
{
	DirectX::XMFLOAT3 scale = transform->GetScale().GetDXVec3();
	DirectX::XMFLOAT3 worldPos = transform->GetWorldPosition().GetDXVec3();
	// 모델의 변환 -> 모델 행렬 결정
	mesh->basicVertexConstantBufferData.model =
		DirectX::SimpleMath::Matrix::CreateScale(scale) *
		DirectX::SimpleMath::Matrix::CreateRotationY(transform->GetRotation().y) *
		DirectX::SimpleMath::Matrix::CreateRotationX(transform->GetRotation().x) *
		DirectX::SimpleMath::Matrix::CreateRotationZ(transform->GetRotation().z) *
		DirectX::SimpleMath::Matrix::CreateTranslation(worldPos);
	mesh->basicVertexConstantBufferData.model = mesh->basicVertexConstantBufferData.model.Transpose();
}

void MeshRendererComponent::UpdateConstantBufferView(Mesh* mesh, Camera* camInst)
{
	DirectX::XMVECTOR viewEyePos = DirectX::XMLoadFloat3(&camInst->GetViewEyePosDX());
	DirectX::XMVECTOR viewEyeDir = DirectX::XMLoadFloat3(&camInst->GetViewEyeDirDX());
	DirectX::XMVECTOR viewUp = DirectX::XMLoadFloat3(&camInst->GetViewUpDX());

	mesh->basicVertexConstantBufferData.view =
		DirectX::XMMatrixLookToLH(viewEyePos, viewEyeDir, viewUp);
	//mesh.basicVertexConstantBufferData.view = 
	//	DirectX::SimpleMath::Matrix::CreateScale(Camera::GetInstance()->GetZoomValue());
	mesh->basicVertexConstantBufferData.view = mesh->basicVertexConstantBufferData.view.Transpose();
}

void MeshRendererComponent::UpdateConstantBufferProjection(Mesh* mesh, Camera* camInst)
{
	float projFOVAngleY = DirectX::XMConvertToRadians(Camera::GetInstance()->GetProjFOVAngleY());
	float aspect = camInst->GetAspect();
	float nearZ = camInst->GetNearZ();
	float farZ = camInst->GetFarZ();
	mesh->basicVertexConstantBufferData.projection =
		DirectX::XMMatrixPerspectiveFovLH(
			projFOVAngleY, aspect, nearZ, farZ
		);
	mesh->basicVertexConstantBufferData.projection =
		mesh->basicVertexConstantBufferData.projection.Transpose();
}
