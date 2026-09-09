// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/Shape/FTShape.h"

#include "TemplateFunctions.h"
#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Mesh/Vertex.h"
#include "ResourceSystem/Mesh/MeshConstantData.h"
#include "ResourceSystem/Mesh/FTMeshData.h"
#include "Utility/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/Camera.h"
#include "Actor/Transform.h"
#include "Manager/DebugShapes.h"
#include "Debugging/DebugFuncs.h"

namespace D3D11
{
	using namespace Core;
	using namespace Math;
	using Microsoft::WRL::ComPtr;

	/// @brief Initializes shape geometry and its rendering state.
	/// @note Initializes the :FTShape base or delegates to its constructor.
	FTShape::FTShape()
		: mMesh(DBG_NEW Mesh)
		, mVSCData()
		, mGSCData()
		, mPSCData()
		, mIsActive(false)
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	FTShape::~FTShape()
	{
		if (mMesh)
		{
			delete mMesh;
			mMesh = nullptr;
		}
		if (mVSCBuf.Get())
			mVSCBuf.Reset();
		if (mGSCBuf.Get())
			mGSCBuf.Reset();
		if (mPSCBuf.Get())
			mPSCBuf.Reset();
	}

	/// @brief Returns the vcdata used by this ftshape.
	/// @return Borrowed access to the vcdata.
	/// @note Changes through the returned reference affect this object's stored state.
	DebugVCData& FTShape::GetVCData() { return mVSCData; }
	/// @brief Returns the gscdata used by this ftshape.
	/// @return Borrowed access to the gscdata.
	/// @note Changes through the returned reference affect this object's stored state.
	DebugGCData& FTShape::GetGSCData() { return mGSCData; }
	/// @brief Returns the pixel constant data used by this ftshape.
	/// @return Borrowed access to the pixel constant data.
	/// @note Changes through the returned reference affect this object's stored state.
	DebugPCData& FTShape::GetPixelConstantData() { return mPSCData; }
	/// @brief Returns the mesh used by this ftshape.
	/// @return Borrowed access to the mesh.
	Mesh*		 FTShape::GetMesh() { return mMesh; }

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param renderer Renderer providing the graphics device and current render state.
	void FTShape::Initialize(D3D11Renderer* renderer)
	{
		InitializeConstantBuffer(renderer->GetDevice());
	}

	/// @brief Uploads vertex-shader constant data.
	/// @param model Model resource associated with this object.
	/// @param camInst Camera supplying the view and projection for this draw.
	void FTShape::UpdateVC(Math::FTMatrix4& model, Core::ICamera* camInst)
	{
		if (!mMesh)
			return;
		mVSCData.model = model.Transposed();
	}

	/// @brief Uploads geometry-shader constant data.
	/// @param camInst Camera supplying the view and projection for this draw.
	void FTShape::UpdateGC(Core::ICamera* camInst)
	{
		Math::FTMatrix4 viewMat = Math::FTMatrix4::Identity;
		Math::FTMatrix4 projMat = Math::FTMatrix4::Identity;
		camInst->GetViewMatrix(viewMat);
		camInst->GetProjectionMatrix(projMat);

		mGSCData.view		= viewMat.Transposed();
		mGSCData.projection = projMat.Transposed();
	}

	/// @brief Uploads pixel-shader constant data.
	void FTShape::UpdatePC()
	{
		mPSCData.IsActive = mIsActive;
	}

	/// @brief Submits this object's graphics work for the current frame.
	/// @param renderer Renderer providing the graphics device and current render state.
	void FTShape::Render(D3D11Renderer* renderer)
	{
		if (!mMesh)
			return;
		if (!mVSCBuf.Get())
			return;
		if (!mPSCBuf.Get())
			return;

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());

		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

		DebugShapes* dbgShapes = DebugShapes::GetInstance();
		context->VSSetShader(dbgShapes->GetVS().Get(), 0, 0);
		context->VSSetConstantBuffers(0, 1, mVSCBuf.GetAddressOf());

		context->GSSetShader(dbgShapes->GetGSSquare().Get(), 0, 0);
		Microsoft::WRL::ComPtr<ID3D11Buffer> GSCBuffers[2] = {
			mVSCBuf,
			mGSCBuf
		};
		context->GSSetConstantBuffers(0, 2, GSCBuffers->GetAddressOf());

		context->PSSetShader(dbgShapes->GetPS().Get(), 0, 0);
		context->PSSetConstantBuffers(0, 1, mPSCBuf.GetAddressOf());

		context->IASetInputLayout(dbgShapes->GetInputLayout().Get());
		context->IASetVertexBuffers(0, 1, mMesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mMesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		context->Draw(mMesh->VertexCount, 0);

		context->GSSetShader(nullptr, 0, 0);
	}

	/// @brief Submits this object's graphics work for the current frame.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @param vertexShader Vertex shader used by the pipeline.
	/// @param pixelShader Pixel shader used by the pipeline.
	/// @param inputLayout Direct3D vertex input layout.
	void FTShape::Render(
		D3D11Renderer*								renderer,
		Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader,
		Microsoft::WRL::ComPtr<ID3D11PixelShader>&	pixelShader,
		Microsoft::WRL::ComPtr<ID3D11InputLayout>&	inputLayout)
	{
		if (!mMesh)
			return;
		if (!mVSCBuf.Get())
			return;
		if (!mPSCBuf.Get())
			return;

		UINT stride = sizeof(DebugVertex);
		UINT offset = 0;

		UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());

		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();
		context->VSSetShader(vertexShader.Get(), 0, 0);
		context->VSSetConstantBuffers(0, 1, mVSCBuf.GetAddressOf());

		context->PSSetShader(pixelShader.Get(), 0, 0);
		context->PSSetConstantBuffers(0, 1, mPSCBuf.GetAddressOf());

		context->IASetInputLayout(inputLayout.Get());
		context->IASetVertexBuffers(0, 1, mMesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mMesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		context->Draw(mMesh->VertexCount, 0);
	}

	/// @brief Builds or binds the mesh resources required by the renderer.
	/// @param device Direct3D device used to create GPU resources.
	/// @param meshData CPU-side mesh vertices and indices.
	void FTShape::InitializeMesh(ComPtr<ID3D11Device>& device, FTDebugMeshData&& meshData)
	{
		mMesh->VertexCount = UINT(meshData.Vertices.GetSize());
		D3D11Utils::CreateVertexBuffer(device, meshData.Vertices, mMesh->VertexBuffer);
	}

	// void FTShape::UpdateConstantBufferModel(Transform* transform)
	//{
	//     int dir = static_cast<int>(transform->GetRightward().x);
	//     FTVector3 worldPos = FTVector3(
	//         transform->GetWorldPosition().x,
	//         transform->GetWorldPosition().y,
	//         transform->GetWorldPosition().z
	//     );
	//     FTVector3 scale = transform->GetScale();
	//     DirectX::XMFLOAT3 scaleWithDir = DirectX::XMFLOAT3(scale.x * dir, scale.y, scale.z);
	//
	//     Matrix model =
	//         Matrix::CreateScale(scaleWithDir) *
	//         Matrix::CreateRotationY(transform->GetRotation().y) *
	//         Matrix::CreateRotationX(transform->GetRotation().x) *
	//         Matrix::CreateRotationZ(transform->GetRotation().z) *
	//         Matrix::CreateTranslation(worldPos.GetDXVec3());
	//     mVSCData.model = model.Transpose();
	// }

	/// @brief Recomputes the model transform used for rendering.
	/// @param pos Position or zero-based insertion index.
	/// @param rot Rotation used by the operation.
	/// @param size Number of elements or bytes required by the operation.
	void FTShape::UpdateModelMatrix(Math::FTVector3 pos, Math::FTVector3 rot, Math::FTVector3 size)
	{
		Math::FTMatrix4 model =
			// Matrix::CreateScale(size.GetDXVec3()) *
			FTMatrix4::CreateRotationY(rot.y) *
			FTMatrix4::CreateRotationX(rot.x) *
			FTMatrix4::CreateRotationZ(rot.z) *
			FTMatrix4::CreateTranslation(pos);
		mVSCData.model = model.Transposed();
	}

	/// @brief Recomputes the camera view matrix from its position and orientation.
	/// @param camInst Camera supplying the view and projection for this draw.
	void FTShape::UpdateViewMatrix(Core::ICamera* camInst)
	{
		Math::FTMatrix4 viewMat = Math::FTMatrix4::Identity;
		camInst->GetViewMatrix(viewMat);
		viewMat.Transpose();
		mGSCData.view = viewMat;
	}

	/// @brief Recomputes the camera projection from its current lens and viewport settings.
	/// @param camInst Camera supplying the view and projection for this draw.
	void FTShape::UpdateProjectionMatrix(Core::ICamera* camInst)
	{
		Math::FTMatrix4 projMat = Math::FTMatrix4::Identity;
		camInst->GetProjectionMatrix(projMat);
		projMat.Transpose();
		mGSCData.projection = projMat;
	}

	/// @brief Allocates and initializes the GPU constant buffer used by this object.
	/// @param device Direct3D device used to create GPU resources.
	void FTShape::InitializeConstantBuffer(ComPtr<ID3D11Device>& device)
	{
		mVSCData.model		= Math::FTMatrix4();
		mGSCData.view		= Math::FTMatrix4();
		mGSCData.projection = Math::FTMatrix4();

		mGSCData.size	  = Math::FTVector2::Zero;
		mPSCData.IsActive = true;

		D3D11Utils::CreateConstantBuffer(device, mVSCData, mVSCBuf);
		D3D11Utils::CreateConstantBuffer(device, mGSCData, mGSCBuf);
		D3D11Utils::CreateConstantBuffer(device, mPSCData, mPSCBuf);
	}

	/// @brief Uploads the current shader parameters to the constant buffers.
	/// @param device Direct3D device used to create GPU resources.
	/// @param context Context associated with this operation.
	void FTShape::UpdateConstantBuffers(
		Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	{
		if (mVSCBuf)
			D3D11Utils::UpdateBuffer(context, mVSCData, mVSCBuf);
		else
			Common::Debug::LogError(__LINE__, __FILE__, "VSC Buffer is null");

		if (mGSCBuf)
			D3D11Utils::UpdateBuffer(context, mGSCData, mGSCBuf);
		else
			Common::Debug::LogError(__LINE__, __FILE__, "GSC Buffer is null");

		if (mPSCBuf)
			D3D11Utils::UpdateBuffer(context, mPSCData, mPSCBuf);
		else
			Common::Debug::LogError(__LINE__, __FILE__, "PSC Buffer is null");
	}
} // namespace D3D11
