#include "FoxtrotEngine/Components/MeshRendererComponent.h"

#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/ResourceSystem/Mesh.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"
#include "FoxtrotEngine/Renderer/Camera.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/Renderer/GeometryGenerator.h"

#ifdef _DEBUG
#include "FoxtrotEditor/FTCoreEditor.h"
#endif

void MeshRendererComponent::Initialize(FTCore* coreInstance)
{
	mRenderer = coreInstance->GetGameRenderer();
}

void MeshRendererComponent::Update(float deltaTime)
{
	if(mMesh)
		UpdateMesh(GetOwner()->GetTransform(), Camera::GetInstance());
}

void MeshRendererComponent::Render(FoxtrotRenderer* renderer){
	if(mMesh)
		RenderMesh(renderer);
}

void MeshRendererComponent::InitializeMesh(MeshData&& meshData)
{
	mMesh = new Mesh;
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
	mRenderer->GetMeshes().emplace_back(mMesh);
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
	, mMesh		(nullptr)
{}

MeshRendererComponent::~MeshRendererComponent()
{}

void MeshRendererComponent::UpdateConstantBufferModel(Mesh* mesh, Transform* transform)
{
	DirectX::XMFLOAT3 scale = transform->GetScale().GetDXVec3();
	DirectX::XMFLOAT3 worldPos = transform->GetWorldPosition().GetDXVec3();
	// ���� ��ȯ -> �� ��� ����
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
	DirectX::XMFLOAT3 eyePos = camInst->GetViewEyePosDX();
	DirectX::XMFLOAT3 eyeDir = camInst->GetViewEyeDirDX();
	DirectX::XMFLOAT3 up = camInst->GetViewUpDX();

	DirectX::XMVECTOR viewEyePos = DirectX::XMLoadFloat3(&eyePos);
	DirectX::XMVECTOR viewEyeDir = DirectX::XMLoadFloat3(&eyeDir);
	DirectX::XMVECTOR viewUp = DirectX::XMLoadFloat3(&up);

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

void MeshRendererComponent::AddCube()
{
	InitializeMesh(GeometryGenerator::MakeBox());
}

void MeshRendererComponent::EditorUpdate(float deltaTime)
{
	Update(deltaTime);
}

void MeshRendererComponent::EditorUIUpdate()
{
	if (ImGui::Button("Add Cube")) {
		AddCube();
	}
}
