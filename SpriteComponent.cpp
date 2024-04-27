#include "SpriteComponent.h"
#include "Actor.h"
#include "Scene.h"
#include "CCore.h"
#include "FTTexture.h"
#include "EditorCamera2D.h"
#include "Transform.h"
#include "ChunkLoader.h"
#include "ResourceManager.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder, int updateOrder)
	: Component(owner, drawOrder)
	, mTextWidth(0)
	, mTexHeight(0)
	, rect(new Bounds)
{
	ResourceManager::GetInstance()->LoadTexture("Asteroid", "./Assets/Asteroid.png");
}

SpriteComponent::~SpriteComponent()
{
	delete rect;
}

MeshData SpriteComponent::MakeSquare()
{
	using DirectX::SimpleMath::Vector2;
	using DirectX::SimpleMath::Vector3;
	using DirectX::SimpleMath::Vector4;

	MeshData output = {};
	
	std::vector<Vector3> positions;
	std::vector<Vector3> colors;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords; // 텍스춰 좌표

	const float scale = 1.0f;

	// 앞면
	positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
	positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
	positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
	positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

	// FTTexture Coordinates (Direct3D 9)
	// https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
	texcoords.push_back(Vector2(0.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 1.0f));

	std::vector<Vertex> vertices;
	for (size_t i = 0; i < positions.size(); i++) {
		Vertex v;
		v.position = positions[i];
		v.color = colors[i];
		v.texcoord = texcoords[i];
		output.vertices.push_back(v);
	}
	output.indices = {
		0, 1, 2, 0, 2, 3, // 앞면
	};

	return output;
}

void SpriteComponent::Initialize(FoxtrotRenderer* renderer, std::string fileName)
{
	mMesh = new Mesh;
	mAspect = renderer->GetAspectRatio();
	// Read Mesh and store it as Mesh Data
	// Implementation needed
	MeshData mesh = MakeSquare();
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

	// 모델의 변환 -> 모델 행렬 결정
	mMesh->basicVertexConstantBufferData.model =
		SimpleMath::Matrix::CreateScale(transform->GetScale().x, transform->GetScale().y, 1.0f) *
		SimpleMath::Matrix::CreateRotationZ(transform->GetRotation()) *
		SimpleMath::Matrix::CreateTranslation(transform->GetWorldPosition().x, transform->GetWorldPosition().y, 0.0f);
	mMesh->basicVertexConstantBufferData.model = mMesh->basicVertexConstantBufferData.model.Transpose();

	// 시점 변환
	// m_constantBufferData.view = XMMatrixLookAtLH(m_viewEye, m_viewFocus,
	// m_viewUp);
	Vector2 lookAtPos = EditorCamera2D::GetInstance()->GetLookAtPos();
	mMesh->basicVertexConstantBufferData.view =
		XMMatrixLookToLH(SimpleMath::Vector3(lookAtPos.x, lookAtPos.y, 0.0f), mViewEyeDir, mViewUp);
	mMesh->basicVertexConstantBufferData.view *= SimpleMath::Matrix::CreateScale(EditorCamera2D::GetInstance()->GetZoomValue());
	mMesh->basicVertexConstantBufferData.view = mMesh->basicVertexConstantBufferData.view.Transpose();

	LogFloat(EditorCamera2D::GetInstance()->GetZoomValue());

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
	BlitToGameview(rect, GetOwner()->GetTransform()->GetScale());
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