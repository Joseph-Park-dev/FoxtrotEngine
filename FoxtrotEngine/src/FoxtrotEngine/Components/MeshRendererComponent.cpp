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

using DXMatrix = DirectX::SimpleMath::Matrix;

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

void MeshRendererComponent::SetTexture(const std::string fileName)
{
	mMesh->texture = new FTTexture;
	mMesh->texture->CreateTexture(mRenderer, fileName);
}

MeshRendererComponent::MeshRendererComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component	(owner, drawOrder, updateOrder)
	, mRenderer	(nullptr)
	, mMesh		(nullptr)
{}

MeshRendererComponent::~MeshRendererComponent(){
	delete mMesh->texture;
}

void MeshRendererComponent::UpdateConstantBufferModel(Mesh* mesh, Transform* transform)
{
	DirectX::XMFLOAT3 scale = transform->GetScale().GetDXVec3();
	DirectX::XMFLOAT3 worldPos = transform->GetWorldPosition().GetDXVec3();
	// 모델의 변환 -> 모델 행렬 결정
	mesh->basicVertexConstantBufferData.model =
		DXMatrix::CreateScale(scale) *
		DXMatrix::CreateRotationY(transform->GetRotation().y) *
		DXMatrix::CreateRotationX(transform->GetRotation().x) *
		DXMatrix::CreateRotationZ(transform->GetRotation().z) *
		DXMatrix::CreateTranslation(worldPos);
	mesh->basicVertexConstantBufferData.model = mesh->basicVertexConstantBufferData.model.Transpose();
}

void MeshRendererComponent::UpdateConstantBufferView(Mesh* mesh, Camera* camInst)
{
	DirectX::XMFLOAT3 eyePosDX = camInst->GetViewEyePosDX();
	DirectX::XMFLOAT3 eyeDir = camInst->GetViewEyeDirDX();
	DirectX::XMFLOAT3 up = camInst->GetViewUpDX();

	DirectX::XMVECTOR viewEyePos = DirectX::XMLoadFloat3(&eyePosDX);
	DirectX::XMVECTOR viewEyeDir = DirectX::XMLoadFloat3(&eyeDir);
	DirectX::XMVECTOR viewUp = DirectX::XMLoadFloat3(&up);

	FTVector3 viewEyeRotation = camInst->GetViewEyeRotation();
	
	mesh->basicVertexConstantBufferData.view =
		DirectX::XMMatrixLookToLH(viewEyePos, viewEyeDir, viewUp);

	mesh->basicVertexConstantBufferData.view *=
		DXMatrix::CreateRotationY(viewEyeRotation.y) *
		DXMatrix::CreateRotationX(viewEyeRotation.x) *
		DXMatrix::CreateRotationZ(viewEyeRotation.z);
		
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
	if (mRenderer->GetViewType() == Viewtype::Perspective) {
		mesh->basicVertexConstantBufferData.projection =
			DirectX::XMMatrixPerspectiveFovLH(
				projFOVAngleY, aspect, nearZ, farZ
			);
	}
	else if (mRenderer->GetViewType() == Viewtype::Orthographic){
		mesh->basicVertexConstantBufferData.projection =
			DirectX::XMMatrixOrthographicOffCenterLH(
				-aspect, aspect, -1.0f, 1.0f, nearZ, farZ
			);
	}
	mesh->basicVertexConstantBufferData.projection =
		mesh->basicVertexConstantBufferData.projection.Transpose();
}

void MeshRendererComponent::AddCube()
{
	if (!mMesh)
	{
		InitializeMesh(GeometryGenerator::MakeBox());
		//SetTexture("./assets/Asteroid.png");
	}
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
