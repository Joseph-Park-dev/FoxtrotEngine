#include "FTSprite.h"

#include "Renderer/D3D11Renderer.h"
#include "Renderer/Camera.h"
#include "Renderer/D3D11Utils.h"
#include "ResourceSystem/D3D11PSO.h"
#include "Manager/ResourceManager.h"
#include "Actor/Transform.h"
#include "Actor/Steering.h"
#include "Math/FTMath.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTGeometryShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/Mesh/MeshConstantData.h"
#include "ResourceSystem/Mesh/Vertex.h"
#include "ResourceSystem/Mesh/Mesh.h"

namespace D3D11
{
	using namespace Core;
	using namespace Math;
	using namespace Microsoft::WRL;
	ResType FTSprite::Type = ResType::SPRITE;

	void FTSprite::UpdateConstantBuffers(D3D11Renderer* renderer, Transform* transform, Camera* camInst, FTMaterial* mat, const size_t gcDataCount, const int meshIndex)
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
		FTVector3 scaleWithDir = FTVector3(scaleX * GetFrontDir() * GetDirection(), scale.y * GetDirection(), scale.z);
		transform->SetWorldScale(scaleWithDir);
		FTMatrix4 modelMat = transform->GetMatrixWorld();

		// Inverse transpose matrix calculation
		// Consider removing this part if the engine is for 2D games.
		// Matrix invTransposeMat = modelMat.Transpose();
		// invTransposeMat.Translation(Vector3(0.0f));
		// invTransposeMat = invTransposeMat.Transpose().Invert();

		// View Transformation
		FTMatrix4&& viewMat = camInst->GetViewRow();

		// Project Transformation
		FTMatrix4&& projMat = std::move(camInst->GetProjRow());

		GetVCData()->ModelMat = modelMat.Transposed();
		D3D11Utils::UpdateBuffer(
			renderer->GetContext(), *GetVCData(), GetVCBuf());

		mGCMatData->ViewMat = viewMat.Transposed();
		mGCMatData->ProjMat = projMat.Transposed();
		D3D11Utils::UpdateBuffer(renderer->GetContext(), *mGCMatData, mGCMatBuf);

		for (size_t i = 0; i < gcDataCount; ++i)
			mGCSpriteData[i].Scale = FTVector2(GetSizeScale().x * scaleWithDir.x, GetSizeScale().y * scaleWithDir.y);

		D3D11Utils::UpdateBuffer(renderer->GetContext(), mGCSpriteData[meshIndex], mGCSpriteBuf);

		if (mat)
			mat->UpdateBuffer(renderer->GetContext());
	}

	void FTSprite::Render(D3D11Renderer* renderer, Transform* transform, Camera* camInst, D3D11PSO* pso, FTMaterial* mat)
	{
		if (!pso->IsValid()) // Vertex Shader is always required when drawing.
			return;

		UINT						 stride	 = sizeof(SpriteVertex);
		UINT						 offset	 = 0;
		Mesh*						 mesh	 = Meshes()->At(0);
		ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

		if (mesh)
		{
			pso->SetToPipeline(context);

			if (mTexture)
			{
				ID3D11ShaderResourceView* const resViews[] = {
					mTexture->GetSRV().Get()
				};
				context->PSSetShaderResources(0, 1, resViews);
			}
			context->VSSetConstantBuffers(
				0, 1, GetVCBuf().GetAddressOf());

			ID3D11Buffer* const gsCBuffers[] = {
				mGCMatBuf.Get(),
				mGCSpriteBuf.Get(),
			};
			context->GSSetConstantBuffers(0, 2, gsCBuffers);

			context->PSSetSamplers(0, 1, GetSamplerState().GetAddressOf());
			if (mat)
				context->PSSetConstantBuffers(0, 1, mat->GetPCBuf().GetAddressOf());

			context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
			context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
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

	FTSprite::FTSprite(FTResourceDef& resDef, D3D11Renderer* renderer)
		: FTMeshGroup(resDef, renderer, nullptr)
		, mGCMatData(DBG_NEW PointVPMat)
		, mGCSpriteData(DBG_NEW SpriteGCData)
		, mTexture(nullptr)
	{
		SpriteVertex* vertex = DBG_NEW SpriteVertex;
		Process(renderer, vertex, 1);
		delete vertex;
	}

	FTSprite::FTSprite(FTResourceDef& resDef, D3D11Renderer* renderer, bool isAnim)
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

	Microsoft::WRL::ComPtr<ID3D11Buffer>& FTSprite::GetGCMatBuf()
	{
		return mGCMatBuf;
	}

	Microsoft::WRL::ComPtr<ID3D11Buffer>& FTSprite::GetGCSpriteBuf()
	{
		return mGCSpriteBuf;
	}

	void FTSprite::Process(D3D11Renderer* renderer, SpriteVertex* vertices, size_t verticesCount)
	{
		if (!Meshes()->IsEmpty())
			return;

		Meshes()->Reserve(1);

		Mesh* mesh = DBG_NEW Mesh;
		D3D11Utils::CreateVertexBuffer(renderer->GetDevice(), vertices, verticesCount, mesh->VertexBuffer);
		mesh->VertexCount = static_cast<UINT>(verticesCount);
		Meshes()->PushBack(mesh);

		CreateTextureSampler(renderer->GetDevice());
		InitializeConstantBuffers(renderer->GetDevice());
	}

#ifdef FOXTROT_EDITOR
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
#endif
} // namespace D3D11