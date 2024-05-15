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
#include "Math.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder)
	, mTextWidth(0)
	, mTexHeight(0)
	, rect(new Bounds)
	, mScale(1.f)
{
	ResourceManager::GetInstance()->LoadTexture("Asteroid", "./Assets/Asteroid.png");
}

SpriteComponent::~SpriteComponent()
{
	FTCore::GetInstance()->GetGameRenderer()->RemoveMesh(mMesh);
	delete rect;
}

void SpriteComponent::SetTexture(FoxtrotRenderer* renderer, std::string fileName)
{
	mMesh->texture = ResourceManager::GetInstance()->GetLoadedTexture(fileName);
	mMesh->texture->CreateTexture(renderer);
}

void SpriteComponent::Initialize(FoxtrotRenderer* renderer)
{
	mMesh = new Mesh;
	mAspect = renderer->GetAspectRatio();
	// Read Mesh and store it as Mesh Data
	// Implementation needed
	MeshData meshData = GeometryGenerator::MakeSquare();
	renderer->CreateVertexBuffer(meshData.vertices, mMesh->vertexBuffer);
	mMesh->indexCount = UINT(meshData.indices.size());
	renderer->CreateIndexBuffer(meshData.indices, mMesh->indexBuffer);

	SetTexture(renderer, "Asteroid");

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

void SpriteComponent::SaveProperties(std::ofstream& ofs)
{
	Component::SaveProperties(ofs);
	//FileIOHelper::AddWString(ofs, mTexture->GetKey());
	//FileIOHelper::AddWString(ofs, mTexture->GetRelativePath());
	FileIOHelper::AddInt(ofs, mTextWidth);
	FileIOHelper::AddInt(ofs, mTexHeight);
}

void SpriteComponent::LoadProperties(std::ifstream& ifs)
{
	Component::LoadProperties(ifs);
	FileIOHelper::LoadInt(ifs, mTextWidth);
	FileIOHelper::LoadInt(ifs, mTexHeight);

	std::wstring textureKey = FileIOHelper::LoadWString(ifs);
	//SetTexture(ResourceManager::GetInstance()->GetLoadedTexture(textureKey));
	std::wstring texturePath = FileIOHelper::LoadWString(ifs);
	//mTexture->SetRelativePath(texturePath);
}

void SpriteComponent::BlitToGameview(Bounds* blitArea, FTVector2 scale)
{
	//FoxtrotRenderer* gameviewRend = FTCore::GetInstance()->GetGameRenderer();
	// 1) Get relative position from mLookAtPos
	Bounds* renderArea = Camera2D::GetInstance()->GetRenderArea();
	FTVector2 renderCenter = FTVector2(renderArea->x, renderArea->y);
	FTVector2 worldPos = GetOwner()->GetTransform()->GetWorldPosition();
	FTVector2 relativePos = worldPos - renderCenter;

	Bounds gameViewRect =
	{
		relativePos.x - GetOwner()->GetTransform()->GetScale().x * 0.5f,
		relativePos.y - GetOwner()->GetTransform()->GetScale().y * 0.5f,
		scale.x,
		scale.y
	};

	/*

		TEXTURE
		RENDER CODE HERE
		(Alternative to SDL_RenderCopy())

	*/
}

void SpriteComponent::BlitToGameviewEx(Bounds* blitArea, FTVector2 scale)
{
	//FoxtrotRenderer* gameviewRend = FTCore::GetInstance()->GetGameRenderer();
	// 1) Get relative position from mLookAtPos
	Bounds* renderArea = Camera2D::GetInstance()->GetRenderArea();
	FTVector2 renderCenter = FTVector2(renderArea->x, renderArea->y);
	FTVector2 worldPos = GetOwner()->GetTransform()->GetWorldPosition();
	FTVector2 relativePos = worldPos - renderCenter;

	Bounds gameViewRect =
	{
		relativePos.x - GetOwner()->GetTransform()->GetScale().x * 0.5f,
		relativePos.y - GetOwner()->GetTransform()->GetScale().y * 0.5f,
		scale.x,
		scale.y
	};

	/*

		TEXTURE
		RENDER CODE HERE
		(Alternative to SDL_RenderCopy())

	*/
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