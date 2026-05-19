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

	FTShape::FTShape()
		: mMesh(DBG_NEW Mesh)
		, mVSCData()
		, mGSCData()
		, mPSCData()
		, mIsActive(false)
	{
	}

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

	DebugVCData& FTShape::GetVCData() { return mVSCData; }
	DebugGCData& FTShape::GetGSCData() { return mGSCData; }
	DebugPCData& FTShape::GetPixelConstantData() { return mPSCData; }
	Mesh*		 FTShape::GetMesh() { return mMesh; }

	void FTShape::Initialize(D3D11Renderer* renderer)
	{
		InitializeConstantBuffer(renderer->GetDevice());
	}

	void FTShape::UpdateVC(Math::FTMatrix4& model, Camera* camInst)
	{
		if (!mMesh)
			return;
		mVSCData.model = model.Transposed();
	}

	void FTShape::UpdateGC(Camera* camInst)
	{
		Math::FTMatrix4 viewMat = Math::FTMatrix4::Identity;
		Math::FTMatrix4 projMat = Math::FTMatrix4::Identity;
		camInst->GetViewMatrix(viewMat);
		camInst->GetProjectionMatrix(projMat);

		mGSCData.view		= viewMat.Transposed();
		mGSCData.projection = projMat.Transposed();
	}

	void FTShape::UpdatePC()
	{
		mPSCData.IsActive = mIsActive;
	}

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

	void FTShape::UpdateViewMatrix(D3D11::Camera* camInst)
	{
		Math::FTMatrix4 viewMat = Math::FTMatrix4::Identity;
		camInst->GetViewMatrix(viewMat);
		viewMat.Transpose();
		mGSCData.view = viewMat;
	}

	void FTShape::UpdateProjectionMatrix(D3D11::Camera* camInst)
	{
		Math::FTMatrix4 projMat = Math::FTMatrix4::Identity;
		camInst->GetProjectionMatrix(projMat);
		projMat.Transpose();
		mGSCData.projection = projMat;
	}

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

	void FTShape::UpdateConstantBuffers(
		Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
	{
		if (mVSCBuf)
			D3D11Utils::UpdateBuffer(context, mVSCData, mVSCBuf);
		else
			Debug::LogError(__LINE__, __FILE__, "VSC Buffer is null");

		if (mGSCBuf)
			D3D11Utils::UpdateBuffer(context, mGSCData, mGSCBuf);
		else
			Debug::LogError(__LINE__, __FILE__, "GSC Buffer is null");

		if (mPSCBuf)
			D3D11Utils::UpdateBuffer(context, mPSCData, mPSCBuf);
		else
			Debug::LogError(__LINE__, __FILE__, "PSC Buffer is null");
	}
} // namespace D3D11