#include "FoxtrotEngine/Components/MeshRendererComponent.h"

#include "FoxtrotEngine/Actors/Transform.h"
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/ResourceSystem/Mesh.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"
#include "FoxtrotEngine/Renderer/Camera.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/Renderer/GeometryGenerator.h"
#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"

#ifdef _DEBUG
#include "FoxtrotEditor/FTCoreEditor.h"
#endif

using DXMatrix = DirectX::SimpleMath::Matrix;

void MeshRendererComponent::Initialize(FTCore* coreInstance){
	mRenderer = coreInstance->GetGameRenderer();
}

void MeshRendererComponent::Update(float deltaTime){
	if(!IsMeshEmpty())
		UpdateMeshArray(GetOwner()->GetTransform(), Camera::GetInstance());
}

void MeshRendererComponent::Render(FoxtrotRenderer* renderer){
	if(!IsMeshEmpty())
		UpdateRenderBuffer(renderer);
}

void MeshRendererComponent::InitializeMesh(std::vector<MeshData>&& meshDataVec){
	if (mMeshArr) {
		SafeDeleteArray(mMeshArr);
	}
	mMeshArr = new Mesh*[meshDataVec.size()];
	assert(!IsMeshEmpty());
	int a = meshDataVec.size();
	for (size_t i = 0; i < meshDataVec.size(); ++i) {
		AddMesh(std::move(meshDataVec[i]), i);
	}
	mRenderer->AddToRenderPool(mMeshArr);
}

void MeshRendererComponent::InitializeMesh(MeshData&& meshData){
	mMeshArr = new Mesh*[1];
	AddMesh(std::move(meshData), 0);
	mRenderer->AddToRenderPool(mMeshArr);
}

void MeshRendererComponent::AddMesh(MeshData&& meshData, size_t index) {
	assert(index < GetArrayLength(mMeshArr));
	Mesh* mesh = new Mesh;
	SetTexture(mesh, ResourceManager::GetInstance()->GetLoadedTexture(meshData.textureKey));
	mRenderer->CreateVertexBuffer(meshData.vertices, mesh->vertexBuffer);
	mesh->indexCount = UINT(meshData.indices.size());
	mRenderer->CreateIndexBuffer(meshData.indices, mesh->indexBuffer);

	// Create Constant buffers
	mesh->basicVertexConstantBufferData.model = DirectX::SimpleMath::Matrix();
	mesh->basicVertexConstantBufferData.view = DirectX::SimpleMath::Matrix();
	mesh->basicVertexConstantBufferData.projection = DirectX::SimpleMath::Matrix();
	mRenderer->CreateConstantBuffer(mesh->basicVertexConstantBufferData,
		mesh->vertexConstantBuffer);
	mRenderer->CreateConstantBuffer(mesh->pixelShaderConstantBufferData,
		mesh->pixelConstantBuffer);
	mMeshArr[index] = mesh;
}

bool MeshRendererComponent::IsMeshEmpty(){
	return GetArrayLength(mMeshArr) < 1 || mMeshArr == nullptr;
}

void MeshRendererComponent::UpdateMeshArray(Transform* transform, Camera* cameraInstance){
	if (!IsMeshEmpty()) {
		for (size_t i = 0; i < GetArrayLength(mMeshArr); ++i) {
			Mesh* mesh = mMeshArr[i];
			UpdateConstantBufferModel(mesh, transform);
			UpdateConstantBufferView(mesh, cameraInstance);
			UpdateConstantBufferProjection(mesh, cameraInstance);
		}
	}
}

void MeshRendererComponent::UpdateRenderBuffer(FoxtrotRenderer* renderer){
	if (!IsMeshEmpty()) {
		for (size_t i = 0; i < GetArrayLength(mMeshArr); ++i) {
			Mesh* mesh = mMeshArr[i];
			renderer->UpdateBuffer(mesh->basicVertexConstantBufferData,
				mesh->vertexConstantBuffer);
			renderer->UpdateBuffer(mesh->pixelShaderConstantBufferData,
				mesh->pixelConstantBuffer);
		}
	}
}

void MeshRendererComponent::SetTexture(Mesh* mesh, FTTexture* texture) {
	if(mesh)
		mesh->texture = texture;
}

MeshRendererComponent::MeshRendererComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component	(owner, drawOrder, updateOrder)
	, mRenderer	(nullptr)
	, mMeshArr	(nullptr)
{}

MeshRendererComponent::~MeshRendererComponent(){
	if (!IsMeshEmpty()) {
		for (size_t i = 0; i < GetArrayLength(mMeshArr); ++i) {
			delete mMeshArr[i]->texture;
			delete mMeshArr[i];
		}
		delete[] mMeshArr;
	}
}

void MeshRendererComponent::UpdateConstantBufferModel(Mesh* mesh, Transform* transform){
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

void MeshRendererComponent::UpdateConstantBufferView(Mesh* mesh, Camera* camInst){
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

void MeshRendererComponent::UpdateConstantBufferProjection(Mesh* mesh, Camera* camInst){
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

void MeshRendererComponent::AddCube(){
	if (IsMeshEmpty())
	{
		InitializeMesh(GeometryGenerator::MakeBox());
		//SetTexture("./assets/Asteroid.png");
	}
}

void MeshRendererComponent::EditorUpdate(float deltaTime){
	Update(deltaTime);
}

void MeshRendererComponent::EditorRender(FoxtrotRenderer* renderer)
{

}

void MeshRendererComponent::EditorUIUpdate(){
	if (ImGui::Button("Add Cube")) {
		AddCube();
	}
}