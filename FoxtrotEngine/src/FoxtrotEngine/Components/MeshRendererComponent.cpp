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
	if (!mMeshKey.empty()) {
		std::vector<MeshData>& meshData = ResourceManager::GetInstance()->GetLoadedMeshes(mMeshKey);
		mMeshGroup->Initialize(meshData, mRenderer->GetDevice(), mRenderer->GetContext());
		if (!mMeshGroup) {
			LogString("ERROR: MeshRendererComponent::InitializeMesh() -> Mesh Init failed");
			return false;
		}
		return true;
	}
	else {
		LogString("ERROR: MeshRendererComponent::InitializeMesh() -> Key doesn't exist");
		return false;
	}
}

bool MeshRendererComponent::InitializeMesh(std::string key)
{
	mMeshKey = key;
	MeshRendererComponent::InitializeMesh();
	return mMeshGroup != nullptr;
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
	if (!mTexKey.empty())
		GetMeshGroup()->SetTexture(mTexKey);
	else 
		LogString("ERROR: MeshRendererComponent::SetTexture() -> Key doesn't exist");
}

MeshRendererComponent::MeshRendererComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component	(owner, drawOrder, updateOrder)
	, mMeshGroup(new FTBasicMeshGroup)
	, mRenderer	 (nullptr)
{}

MeshRendererComponent::~MeshRendererComponent(){
	if (mMeshGroup)
		delete mMeshGroup;
}

void MeshRendererComponent::UpdateConstantBufferModel(Transform* transform){
	DirectX::XMFLOAT3 scale = transform->GetScale().GetDXVec3();
	DirectX::XMFLOAT3 worldPos = transform->GetWorldPosition().GetDXVec3();
	// ���� ��ȯ -> �� ��� ����
	Matrix model =
		DXMatrix::CreateScale(scale) *
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

void MeshRendererComponent::SaveProperties(nlohmann::ordered_json& out)
{
	Component::SaveProperties(out);
	FileIOHelper::AddScalarValue(out["Key"], mMeshKey);
}

void MeshRendererComponent::EditorUpdate(float deltaTime){
	Update(deltaTime);
}

void MeshRendererComponent::EditorRender(FoxtrotRenderer* renderer)
{
	Render(renderer);
}

void MeshRendererComponent::EditorUIUpdate(){
	if (ImGui::Button("Add Cube")) {
		//mMeshGroup = ResourceManager::GetInstance()->GetLoadedMeshes("Box");
		LogString("Cube added");
	}
}