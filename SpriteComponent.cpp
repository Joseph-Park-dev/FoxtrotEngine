#include "SpriteComponent.h"
#include "Actor.h"
#include "Scene.h"
#include "CCore.h"
#include "FTTexture.h"
#include "EditorCamera2D.h"
#include "Transform.h"
#include "ChunkLoader.h"
#include "ResourceManager.h"
#include "MeshData.h"
#include "GeometryGenerator.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder)
	, mTextWidth(0)
	, mTexHeight(0)
	, rect(new Bounds)
	, mScale(.5f)
{
	ResourceManager::GetInstance()->LoadTexture("Asteroid", "./Assets/Asteroid.png");
}

SpriteComponent::~SpriteComponent()
{
	CCore::GetInstance()->GetEditorRenderer()->RemoveMesh(mMesh);
	delete rect;
}

void SpriteComponent::Initialize(FoxtrotRenderer* renderer, std::string fileName)
{
	mMesh = new Mesh;
	mAspect = renderer->GetAspectRatio();
	// Read Mesh and store it as Mesh Data
	// Implementation needed
	MeshData mesh = GeometryGenerator::MakeSquare();
	renderer->CreateVertexBuffer(mesh.vertices, mMesh->vertexBuffer);
	mMesh->m_indexCount = UINT(mesh.indices.size());
	renderer->CreateIndexBuffer(mesh.indices, mMesh->indexBuffer);

	mMesh->texture = ResourceManager::GetInstance()->GetLoadedTexture(fileName);
	mMesh->texture->CreateTexture(renderer);

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
	using namespace DirectX;

	Transform* transform = GetOwner()->GetTransform();
	Vector2 lookAtPos = EditorCamera2D::GetInstance()->GetLookAtPos();
	float cameraMouseNavFactor = EditorCamera2D::GetInstance()->GetMouseNavFactor();
	Vector2 scale = transform->GetScale();
	Vector2 worldPos = transform->GetWorldPosition() * cameraMouseNavFactor;
	// 모델의 변환 -> 모델 행렬 결정
	mMesh->basicVertexConstantBufferData.model =
		SimpleMath::Matrix::CreateScale(scale.x, scale.y, 1.0f) *
		SimpleMath::Matrix::CreateRotationZ(transform->GetRotation()) *
		SimpleMath::Matrix::CreateTranslation(worldPos.x, worldPos.y, 0.0f);
	mMesh->basicVertexConstantBufferData.model = mMesh->basicVertexConstantBufferData.model.Transpose();

	// 시점 변환
	// m_constantBufferData.view = XMMatrixLookAtLH(m_viewEye, m_viewFocus,
	// m_viewUp);
	mMesh->basicVertexConstantBufferData.view =
		XMMatrixLookToLH(SimpleMath::Vector3(lookAtPos.x, lookAtPos.y, 0.0f), mViewEyeDir, mViewUp);
	mMesh->basicVertexConstantBufferData.view *= SimpleMath::Matrix::CreateScale(EditorCamera2D::GetInstance()->GetZoomValue());
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

	mMesh->basicVertexConstantBufferData.projection = XMMatrixOrthographicOffCenterLH(
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
		renderer->UpdateBuffer(mMesh->basicVertexConstantBufferData,
			mMesh->vertexConstantBuffer);
		renderer->UpdateBuffer(mMesh->basicVertexConstantBufferData,
			mMesh->pixelConstantBuffer);
	}
	else
		Initialize(renderer, "Asteroid");
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

void SpriteComponent::BlitToGameview(Bounds* blitArea, Vector2 scale)
{
	//FoxtrotRenderer* gameviewRend = CCore::GetInstance()->GetGameRenderer();
	// 1) Get relative position from mLookAtPos
	Bounds* renderArea = Camera2D::GetInstance()->GetRenderArea();
	Vector2 renderCenter = Vector2(renderArea->x, renderArea->y);
	Vector2 worldPos = GetOwner()->GetTransform()->GetWorldPosition();
	Vector2 relativePos = worldPos - renderCenter;

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

void SpriteComponent::BlitToGameviewEx(Bounds* blitArea, Vector2 scale)
{
	//FoxtrotRenderer* gameviewRend = CCore::GetInstance()->GetGameRenderer();
	// 1) Get relative position from mLookAtPos
	Bounds* renderArea = Camera2D::GetInstance()->GetRenderArea();
	Vector2 renderCenter = Vector2(renderArea->x, renderArea->y);
	Vector2 worldPos = GetOwner()->GetTransform()->GetWorldPosition();
	Vector2 relativePos = worldPos - renderCenter;

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