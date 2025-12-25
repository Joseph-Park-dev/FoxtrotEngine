#include "FTSprite.h"

#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/FTShaders/FTGeometryShader.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "Managers/ResourceManager.h"

void FTSprite::Render(
	FoxtrotRenderer*  renderer,
	Transform*		  transform,
	Camera*			  camInst,
	FTVertexShader*	  vs,
	FTGeometryShader* gs,
	FTPixelShader*	  ps,
	FTMaterial*		  mat)
{
	// This enables the resource reusable throughout the Component instances.
	UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext(), transform, camInst, mat, GetFrontDir());

	if (!vs || !ps || !gs || !mat) // Vertex Shader is always required when drawing.
		return;

	UINT						 stride	 = sizeof(SpriteVertex);
	UINT						 offset	 = 0;
	Mesh*						 mesh	 = Meshes()->At(0);
	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	if (mesh)
	{
		if (mTexture)
		{
			ID3D11ShaderResourceView* const resViews[] = {
				mTexture->GetSRV().Get()
			};
			context->PSSetShaderResources(0, 1, resViews);
		}

		context->VSSetShader(vs->GetShader().Get(), 0, 0);
		context->VSSetConstantBuffers(
			0, 1, GetVCBuf().GetAddressOf());

		context->GSSetShader(gs->GetShader().Get(), 0, 0);

		ID3D11Buffer* const gsCBuffers[] = {
			mGCMatBuf.Get(),
			mGCSpriteBuf.Get(),
		};
		context->GSSetConstantBuffers(0, 2, gsCBuffers);

		context->PSSetShader(ps->GetShader().Get(), 0, 0);
		context->PSSetSamplers(0, 1, GetSamplerState().GetAddressOf());
		if (mat)
			context->PSSetConstantBuffers(0, 1, mat->GetPCBuf().GetAddressOf());

		context->IASetInputLayout(vs->GetInputLayout().Get());
		context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		context->Draw(1, 0);
	}
}

FTTexture* FTSprite::GetTexture() const
{
	return mTexture;
}

SpriteGCData* FTSprite::GetGCSpriteData() const
{
	return mGCSpriteData;
}

void FTSprite::SetTexture(FTTexture* texture)
{
	mTexture = texture;
}

void FTSprite::SetGCSpriteData(SpriteGCData* data)
{
	mGCSpriteData = data;
}

FTSprite::FTSprite(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTMeshGroup(resDef, renderer, nullptr)
	, mGCMatData(DBG_NEW PointVPMat)
	, mGCSpriteData(DBG_NEW SpriteGCData)
	, mTexture(nullptr)
{
	SpriteVertex* vertex = DBG_NEW SpriteVertex;
	Process(renderer, vertex, 1);
	delete vertex;
}

FTSprite::FTSprite(FTResourceDef& resDef, FoxtrotRenderer* renderer, bool isAnim)
	: FTMeshGroup(resDef, renderer, nullptr)
	, mGCMatData(DBG_NEW PointVPMat)
	, mGCSpriteData(DBG_NEW SpriteGCData)
	, mTexture(nullptr)
{
	if (!isAnim)
	{
		SpriteVertex* vertex = DBG_NEW SpriteVertex;
		Process(renderer, vertex, 1);
		delete vertex;
	}
}

FTSprite::~FTSprite()
{
	delete mGCMatData;
	delete[] mGCSpriteData;
	if (mTexture)
	{
		delete mTexture;
		mTexture = nullptr;
	}
}

void FTSprite::InitializeConstantBuffers(ComPtr<ID3D11Device>& device)
{
	FTMeshGroup::InitializeConstantBuffers(device);
	D3D11Utils::CreateConstantBuffer(device, *mGCMatData, mGCMatBuf);
	D3D11Utils::CreateConstantBuffer(device, *mGCSpriteData, mGCSpriteBuf);
}

void FTSprite::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, Transform* transform, Camera* camInst, FTMaterial* mat, const int frontDir, const size_t gcDataCount)
{
	// Model Transformation
	// Front Direction will be multiplied to scale.
	// When frontDir is minus, multiplication must be done only once as the character switches direction.

	float linearX = transform->GetSteering()->Linear.x;
	if (linearX < 0)
		SetDirection(-1);
	else if (0 < linearX)
		SetDirection(1);

	FTVector3 scale		   = transform->GetWorldScale();
	float	  scaleX	   = Math::Abs(scale.x);
	FTVector3 scaleWithDir = FTVector3(scaleX * frontDir * GetDirection(), scale.y * GetDirection(), scale.z);
	transform->SetWorldScale(scaleWithDir);
	Matrix modelMat = transform->GetMatrixWorld();

	// Inverse transpose matrix calculation
	// Consider removing this part if the engine is for 2D games.
	// Matrix invTransposeMat = modelMat.Transpose();
	// invTransposeMat.Translation(Vector3(0.0f));
	// invTransposeMat = invTransposeMat.Transpose().Invert();

	// View Transformation
	Matrix&& viewMat = camInst->GetViewRow();

	// Project Transformation
	Matrix&& projMat = std::move(camInst->GetProjRow());

	GetVCData()->ModelMat = modelMat.Transpose();
	D3D11Utils::UpdateBuffer(
		context, *GetVCData(), GetVCBuf());

	mGCMatData->ViewMat = viewMat.Transpose();
	mGCMatData->ProjMat = projMat.Transpose();
	D3D11Utils::UpdateBuffer(context, *mGCMatData, mGCMatBuf);

	for (size_t i = 0; i < gcDataCount; ++i)
	{
		mGCSpriteData[i].Scale = Vector2(GetSizeScale().x * scaleWithDir.x, GetSizeScale().y * scaleWithDir.y);
	}
	D3D11Utils::UpdateBuffer(context, *mGCSpriteData, mGCSpriteBuf);

	if (mat)
		mat->UpdateBuffer(context);
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& FTSprite::GetGCMatBuf()
{
	return mGCMatBuf;
}

Microsoft::WRL::ComPtr<ID3D11Buffer>& FTSprite::GetGCSpriteBuf()
{
	return mGCSpriteBuf;
}

void FTSprite::Process(FoxtrotRenderer* renderer, SpriteVertex* vertices, size_t verticesCount)
{
	if (this->IsProcessed())
		return;

	if (this->GetRelativePath().IsEmpty())
		return;

	Meshes()->Reserve(1);

	Mesh* mesh = DBG_NEW Mesh;
	D3D11Utils::CreateVertexBuffer(renderer->GetDevice(), vertices, verticesCount, mesh->VertexBuffer);
	mesh->VertexCount = verticesCount;
	Meshes()->PushBack(mesh);

	CreateTextureSampler(renderer->GetDevice());
	InitializeConstantBuffers(renderer->GetDevice());

	FTResource::Process();
}

void FTSprite::UpdateUI()
{
	mTexture->UpdateUI();
	mGCSpriteData->UpdateUI();
	FTMeshGroup::UpdateUI();
}

void FTSprite::AddRefCount()
{
	mTexture->AddRefCount();
}

void FTSprite::SubtractRefCount()
{
	mTexture->SubtractRefCount();
}
