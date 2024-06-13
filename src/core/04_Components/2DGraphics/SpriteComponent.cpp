#include "SpriteComponent.h"
#include "Actor.h"
#include "Scene.h"
#include "FTCore.h"
#include "FTTexture.h"
#include "EditorCamera2D.h"
#include "Transform.h"
#include "ChunkLoader.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "MeshData.h"
#include "GeometryGenerator.h"
#include "FTMath.h"
#include "CommandHistory.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder)
	, mTexWidth(0)
	, mTexHeight(0)
	, mChannel(4)
	, rect(new Bounds)
	, mScale(1.f)
	, mMesh(nullptr)
{}

SpriteComponent::~SpriteComponent()
{
	FTCore::GetInstance()->GetGameRenderer()->RemoveMesh(mMesh);
	delete rect;
	mRenderer = nullptr;
}

void SpriteComponent::SetTexture(FoxtrotRenderer* renderer, std::string fileName)
{
	std::vector<Mesh*>& meshes = renderer->GetMeshes();
	std::vector<Mesh*>::iterator iter = std::find(meshes.begin(), meshes.end(), mMesh);
	if (iter != meshes.end())
	{
		ResourceManager::GetInstance()->LoadTexture(renderer, fileName);
		mMesh->texture = ResourceManager::GetInstance()->GetLoadedTexture(fileName);
	}
}

void SpriteComponent::UpdateTexture(FoxtrotRenderer* renderer, std::string fileName)
{
	std::vector<Mesh*>& meshes = renderer->GetMeshes();
	std::vector<Mesh*>::iterator iter = std::find(meshes.begin(), meshes.end(), mMesh);
	if (iter != meshes.end())
	{
		delete (*iter);
		meshes.erase(iter);

		//FTTexture* texture = (*iter)->texture;
		//ResourceManager::GetInstance()->UpdateTexture(renderer, texture, channels);
		//mMesh->texture = ResourceManager::GetInstance()->GetLoadedTexture(fileName);

		Initialize(renderer);
		SetTexture(renderer, fileName);
	}
}

void SpriteComponent::Initialize(FoxtrotRenderer* renderer)
{
	mRenderer = renderer;
	mMesh = new Mesh;
	mAspect = renderer->GetAspectRatio();
	// Read Mesh and store it as Mesh Data
	// Implementation needed
	MeshData meshData = GeometryGenerator::MakeSquare(mScale);
	renderer->CreateVertexBuffer(meshData.vertices, mMesh->vertexBuffer);
	mMesh->indexCount = UINT(meshData.indices.size());
	renderer->CreateIndexBuffer(meshData.indices, mMesh->indexBuffer);

	// Create Constant buffers
	mMesh->basicVertexConstantBufferData.model = DirectX::SimpleMath::Matrix();
	mMesh->basicVertexConstantBufferData.view = DirectX::SimpleMath::Matrix();
	mMesh->basicVertexConstantBufferData.projection = DirectX::SimpleMath::Matrix();
	renderer->CreateConstantBuffer(mMesh->basicVertexConstantBufferData,
		mMesh->vertexConstantBuffer);
	renderer->CreateConstantBuffer(mMesh->pixelShaderConstantBufferData,
		mMesh->pixelConstantBuffer);
	renderer->GetMeshes().emplace_back(mMesh);
}

void SpriteComponent::Update(float deltaTime)
{
	Transform* transform = GetOwner()->GetTransform();
	float cameraMouseNavFactor = EditorCamera2D::GetInstance()->GetMouseNavFactor();
	FTVector2 lookAtPos = EditorCamera2D::GetInstance()->GetLookAtPos() * cameraMouseNavFactor;
	FTVector2 scale = transform->GetScale();
	FTVector2 worldPos = transform->GetWorldPosition();
	// 모델의 변환 -> 모델 행렬 결정
	mMesh->basicVertexConstantBufferData.model =
		DirectX::SimpleMath::Matrix::CreateScale(scale.x, scale.y, 1.0f) *
		DirectX::SimpleMath::Matrix::CreateRotationZ(transform->GetRotation()) *
		DirectX::SimpleMath::Matrix::CreateTranslation(worldPos.x, worldPos.y, 0.0f);
	mMesh->basicVertexConstantBufferData.model = mMesh->basicVertexConstantBufferData.model.Transpose();

	// 시점 변환
	// m_constantBufferData.view = XMMatrixLookAtLH(m_viewEye, m_viewFocus,
	// m_viewUp);
	mMesh->basicVertexConstantBufferData.view =
		XMMatrixLookToLH(DirectX::SimpleMath::Vector3(lookAtPos.x, lookAtPos.y, 0.0f), mViewEyeDir, mViewUp);
	mMesh->basicVertexConstantBufferData.view *= DirectX::SimpleMath::Matrix::CreateScale(EditorCamera2D::GetInstance()->GetZoomValue());
	mMesh->basicVertexConstantBufferData.view = mMesh->basicVertexConstantBufferData.view.Transpose();

	// 프로젝션	
	// m_aspect = AppBase::GetAspectRatio(); // <- GUI에서 조절
	/*if (m_usePerspectiveProjection) {
		mBasicVertexConstantBufferData.projection = XMMatrixPerspectiveFovLH(
			XMConvertToRadians(mProjFovAngleY), mAspect, mNearZ, mFarZ);
	}
	else {
		mBasicVertexConstantBufferData.projection = XMMatrixOrthographicOffCenterLH(
			-mAspect, mAspect, -1.0f, 1.0f, mNearZ, mFarZ);
	}*/

	mMesh->basicVertexConstantBufferData.projection = DirectX::XMMatrixOrthographicOffCenterLH(
		-mAspect, mAspect, -1.0f, 1.0f, mNearZ, mFarZ);
	mMesh->basicVertexConstantBufferData.projection =
		mMesh->basicVertexConstantBufferData.projection.Transpose();

	// Constant를 CPU에서 GPU로 복사
	// Render() 함수에서...
}

#ifdef _DEBUG
void SpriteComponent::Render(FoxtrotRenderer* renderer)
{
	if (mMesh) {
		mAspect = renderer->GetAspectRatio();
		renderer->UpdateBuffer(mMesh->basicVertexConstantBufferData,
			mMesh->vertexConstantBuffer);
		renderer->UpdateBuffer(mMesh->pixelShaderConstantBufferData,
			mMesh->pixelConstantBuffer);
	}
	else
		Initialize(renderer);
	//BlitToGameview(rect, GetOwner()->GetTransform()->GetScale());
}
//void SpriteComponent::SetTexture()
//{
//	//rect->x = (int)GetOwner()->GetTransform()->GetWorldPosition().x;
//	//rect->y = (int)GetOwner()->GetTransform()->GetWorldPosition().y;
//	//rect->w = mTextWidth;
//	//rect->h = mTexHeight;
//}

void SpriteComponent::EditorUpdate(float deltaTime)
{
	Update(deltaTime);
}

void SpriteComponent::EditorUIUpdate()
{
	if (mRenderer)
	{
		UpdateSprite(mRenderer);
		ImGui::SeparatorText("Sprite Size");
		UpdateScale();
		if (ImGui::Button("Apply"))
		{
			if(mMesh && mMesh->texture)
				UpdateTexture(mRenderer, mMesh->texture->GetRelativePath());
		}
	}
}

void SpriteComponent::UpdateSprite(FoxtrotRenderer* renderer)
{
	std::string currentSprite = "No sprite has been assigned";
	if (mMesh->texture)
		currentSprite = "Current sprite : \n" + mMesh->texture->GetRelativePath();
	ImGui::Text(currentSprite.c_str());

	if (ImGui::Button("Select Sprite"))
	{
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		ImGuiFileDialog::Instance()->OpenDialog("SelectSprite", "Select Sprite", SPRITE_FORMAT_SUPPORTED, config);
	}

	if (ImGuiFileDialog::Instance()->Display("SelectSprite"))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			std::string spritePath = ImGuiFileDialog::Instance()->GetCurrentPath() + "\\" +
				ImGuiFileDialog::Instance()->GetCurrentFileName();
			SetTexture(renderer, spritePath);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}

void SpriteComponent::UpdateTexWidth()
{
	CommandHistory::GetInstance()->UpdateIntValue("Texture Width", &mTexWidth, INTMOD_SPEED);
}

void SpriteComponent::UpdateTexHeight()
{
	CommandHistory::GetInstance()->UpdateIntValue("Texture Height", &mTexHeight, INTMOD_SPEED);
}

void SpriteComponent::UpdateScale()
{
	CommandHistory::GetInstance()->UpdateFloatValue("Texture Scale", &mScale, FLOATMOD_SPEED);
}

void SpriteComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	//FileIOHelper::AddWString(ofs, mTexture->GetKey());
	//FileIOHelper::AddWString(ofs, mTexture->GetRelativePath());
	FileIOHelper::AddFloat(ofs, mTexWidth);
	FileIOHelper::AddInt(ofs, mTexHeight);

	if(mMesh->texture)
		FileIOHelper::AddBasicString(ofs, mMesh->texture->GetRelativePath());
}

void SpriteComponent::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
	FileIOHelper::LoadInt(ifs, mTexWidth);
	FileIOHelper::LoadInt(ifs, mTexHeight);

	if (mMesh && mMesh->texture)
	{
		std::string fileName = FileIOHelper::LoadBasicString(ifs);
		SetTexture(mRenderer, fileName);
	}
}

#else
void SpriteComponent::Render(FoxtrotRenderer* renderer)
{
	if (mTexture)
	{
		SDL_RenderCopyEx(renderer,
			GetTexture()->GetGameviewTexture(),
			nullptr,
			rect,
			-Math::ToDegrees(GetOwner()->GetTransform()->GetRotation()),
			nullptr,
			SDL_FLIP_NONE
		);
	}
}

void SpriteComponent::SetTexture(FTTexture* texture)
{
	mTexture = texture;
	SDL_QueryTexture(mTexture->GetGameviewTexture(), nullptr, nullptr, &mTextWidth, &mTexHeight);
	rect = new SDL_Rect;
	rect->x = (int)GetOwner()->GetTransform()->GetWorldPosition().x;
	rect->y = (int)GetOwner()->GetTransform()->GetWorldPosition().y;
	rect->w = mTextWidth;
	rect->h = mTexHeight;
}
#endif