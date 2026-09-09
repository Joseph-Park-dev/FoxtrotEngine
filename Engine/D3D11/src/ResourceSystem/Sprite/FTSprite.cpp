#include "Renderer/D3D11Renderer.h"
#include "ResourceSystem/Sprite/FTSprite.h"

#include "Renderer/IRenderer.h"
#include "Renderer/ICamera.h"
#include "Utility/D3D11Utils.h"
#include "ResourceSystem/D3D11PSO.h"
#include "Manager/ResourceManager.h"
#include "Actor/Transform.h"
#include "Actor/Steering.h"
#include "FTMath.h"
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

	/// @brief Uploads the current shader parameters to the constant buffers.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param transform Transform associated with the actor.
	/// @param camInst Camera supplying the view and projection for this draw.
	/// @param mat Matrix or material used by this operation.
	/// @param gcDataCount Number of geometry-shader constant-data entries.
	/// @param meshIndex Index of the mesh within the group.
	void FTSprite::UpdateConstantBuffers(Core::IRenderer* renderer, Transform* transform, Core::ICamera* camInst, FTMaterial* mat, const size_t gcDataCount, const int meshIndex)
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
		FTMatrix4 viewMat = FTMatrix4::Identity;
		camInst->GetViewMatrix(viewMat);

		// Project Transformation
		FTMatrix4 projMat = FTMatrix4::Identity;
		camInst->GetProjectionMatrix(projMat);

		GetVCData()->ModelMat = modelMat.Transposed();
		D3D11Utils::UpdateBuffer(
			static_cast<D3D11Renderer*>(renderer)->GetContext(), *GetVCData(), GetVCBuf());

		mGCMatData->ViewMat = viewMat.Transposed();
		mGCMatData->ProjMat = projMat.Transposed();
		D3D11Utils::UpdateBuffer(static_cast<D3D11Renderer*>(renderer)->GetContext(), *mGCMatData, mGCMatBuf);

		for (size_t i = 0; i < gcDataCount; ++i)
			mGCSpriteData[i].Scale = FTVector2(GetSizeScale().x * scaleWithDir.x, GetSizeScale().y * scaleWithDir.y);

		D3D11Utils::UpdateBuffer(static_cast<D3D11Renderer*>(renderer)->GetContext(), mGCSpriteData[meshIndex], mGCSpriteBuf);

		if (mat)
			mat->UpdateBuffer(static_cast<D3D11Renderer*>(renderer)->GetContext());
	}

	/// @brief Submits this object's graphics work for the current frame.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param transform Transform associated with the actor.
	/// @param camInst Camera supplying the view and projection for this draw.
	/// @param pso Pipeline state object used for rendering.
	/// @param mat Matrix or material used by this operation.
	void FTSprite::Render(Core::IRenderer* renderer, Transform* transform, Core::ICamera* camInst, D3D11PSO* pso, FTMaterial* mat)
	{
		if (!pso->IsValid()) // Vertex Shader is always required when drawing.
			return;

		UINT						 stride	 = sizeof(SpriteVertex);
		UINT						 offset	 = 0;
		Mesh*						 mesh	 = Meshes()->At(0);
		ComPtr<ID3D11DeviceContext>& context = static_cast<D3D11Renderer*>(renderer)->GetContext();

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

	/// @brief Returns the texture used by this ftsprite.
	/// @return Borrowed access to the texture.
	FTTexture* FTSprite::GetTexture() const
	{
		return mTexture;
	}

	/// @brief Returns the gcsprite data used by this ftsprite.
	/// @return Borrowed access to the gcsprite data.
	SpriteGCData* FTSprite::GetGCSpriteData() const
	{
		return mGCSpriteData;
	}

	/// @brief Updates the texture used by subsequent operations.
	/// @param texture Replacement texture.
	void FTSprite::SetTexture(FTTexture* texture)
	{
		mTexture = texture;
	}

	/// @brief Updates the gcsprite data used by subsequent operations.
	/// @param data Replacement gcsprite data.
	void FTSprite::SetGCSpriteData(SpriteGCData* data)
	{
		mGCSpriteData = data;
	}

	/// @brief Initializes sprite metadata and its mesh representation.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @note Initializes the :FTSprite base or delegates to its constructor.
	FTSprite::FTSprite(Common::FTResourceDef& resDef, void* renderer)
		: FTMeshGroup(resDef, renderer, nullptr)
		, mGCMatData(DBG_NEW PointVPMat)
		, mGCSpriteData(DBG_NEW SpriteGCData)
		, mTexture(nullptr)
	{
		SpriteVertex* vertex = DBG_NEW SpriteVertex;
		Process(reinterpret_cast<Core::IRenderer*>(renderer), vertex, 1);
		delete vertex;
	}

	/// @brief Initializes sprite metadata and its mesh representation.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param isAnim Whether the resource uses animated rendering.
	/// @note Initializes the :FTSprite base or delegates to its constructor.
	FTSprite::FTSprite(Common::FTResourceDef& resDef, void* renderer, bool isAnim)
		: FTMeshGroup(resDef, renderer, nullptr)
		, mGCMatData(DBG_NEW PointVPMat)
		, mGCSpriteData(DBG_NEW SpriteGCData)
		, mTexture(nullptr)
	{
		if (!isAnim)
		{
			SpriteVertex* vertex = DBG_NEW SpriteVertex;
			Process(reinterpret_cast<Core::IRenderer*>(renderer), vertex, 1);
			delete vertex;
		}
	}

	/// @brief Releases the resources managed by this instance during destruction.
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

	/// @brief Allocates and initializes the GPU constant buffers used by this object.
	/// @param device Direct3D device used to create GPU resources.
	void FTSprite::InitializeConstantBuffers(ComPtr<ID3D11Device>& device)
	{
		FTMeshGroup::InitializeConstantBuffers(device);
		D3D11Utils::CreateConstantBuffer(device, *mGCMatData, mGCMatBuf);
		D3D11Utils::CreateConstantBuffer(device, *mGCSpriteData, mGCSpriteBuf);
	}

	/// @brief Returns the gcmat buf used by this ftsprite.
	/// @return Borrowed access to the gcmat buf.
	/// @note Changes through the returned reference affect this object's stored state.
	Microsoft::WRL::ComPtr<ID3D11Buffer>& FTSprite::GetGCMatBuf()
	{
		return mGCMatBuf;
	}

	/// @brief Returns the gcsprite buf used by this ftsprite.
	/// @return Borrowed access to the gcsprite buf.
	/// @note Changes through the returned reference affect this object's stored state.
	Microsoft::WRL::ComPtr<ID3D11Buffer>& FTSprite::GetGCSpriteBuf()
	{
		return mGCSpriteBuf;
	}

	/// @brief Applies this object's processing step.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param vertices Vertex data defining the mesh.
	/// @param verticesCount Number of vertices.
	void FTSprite::Process(Core::IRenderer* renderer, SpriteVertex* vertices, size_t verticesCount)
	{
		if (!Meshes()->IsEmpty())
			return;

		Meshes()->Reserve(1);

		Mesh* mesh = DBG_NEW Mesh;
		D3D11Utils::CreateVertexBuffer(static_cast<D3D11Renderer*>(renderer)->GetDevice(), vertices, verticesCount, mesh->VertexBuffer);
		mesh->VertexCount = static_cast<UINT>(verticesCount);
		Meshes()->PushBack(mesh);

		CreateTextureSampler(static_cast<D3D11Renderer*>(renderer)->GetDevice());
		InitializeConstantBuffers(static_cast<D3D11Renderer*>(renderer)->GetDevice());
	}

#ifdef FOXTROT_EDITOR
	/// @brief Builds the editor controls for inspecting and modifying this object's state.
	void FTSprite::UpdateUI()
	{
		mTexture->UpdateUI();
		mGCSpriteData->UpdateUI();
		FTMeshGroup::UpdateUI();
	}

	/// @brief Increments the resource metadata's reference count when metadata exists.
	void FTSprite::AddRefCount()
	{
		mTexture->AddRefCount();
	}

	/// @brief Decrements the resource metadata's reference count when metadata exists.
	void FTSprite::SubtractRefCount()
	{
		mTexture->SubtractRefCount();
	}
#endif
} // namespace D3D11
