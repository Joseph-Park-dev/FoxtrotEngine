// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/FTShape.h"

#include <directxtk/SimpleMath.h>

#include "Renderer/D3D11Utils.h"
#include "Core/TemplateFunctions.h"
#include "ResourceSystem/GeometryGenerator.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "Actors/Transform.h"
#include "Managers/DebugShapes.h"

using Matrix = DirectX::SimpleMath::Matrix;

FTShape::FTShape()
	: mMesh(nullptr)
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
	if (mVertexConstantBuffer.Get())
		mVertexConstantBuffer.Reset();
	if (mPixelConstantBuffer.Get())
		mPixelConstantBuffer.Reset();
}

DebugVCData& FTShape::GetVCData() { return mVSCData; }
DebugGCData& FTShape::GetGSCData() { return mGSCData; }
DebugPCData& FTShape::GetPixelConstantData() { return mPSCData; }
Mesh*		 FTShape::GetMesh() { return mMesh; }

void FTShape::Initialize(FoxtrotRenderer* renderer)
{
	InitializeConstantBuffer(renderer->GetDevice());
}

void FTShape::UpdateVC(Transform* transform, Camera* camInst)
{
	if (!mMesh)
		return;

	Matrix&& modelMat = Matrix();
	modelMat		  = transform->GetMatrixWorld();
	mVSCData.model	  = modelMat.Transpose();
}

void FTShape::UpdateGC(Camera* camInst)
{
	Matrix&& viewMat	= camInst->GetViewRow();
	Matrix&& projMat	= camInst->GetProjRow();
	mGSCData.view		= viewMat.Transpose();
	mGSCData.projection = projMat.Transpose();
}

void FTShape::UpdatePC()
{
	mPSCData.IsActive = mIsActive;
}

void FTShape::Render(FoxtrotRenderer* renderer)
{
	if (!mMesh)
		return;
	if (!mVertexConstantBuffer.Get())
		return;
	if (!mPixelConstantBuffer.Get())
		return;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());

	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	DebugShapes* dbgShapes = DebugShapes::GetInstance();
	context->VSSetShader(dbgShapes->GetVS().Get(), 0, 0);
	context->VSSetConstantBuffers(0, 1, mMesh->VertexConstantBuffers.at(0).GetAddressOf());

	context->GSSetShader(dbgShapes->GetGSSquare().Get(), 0, 0);
	ComPtr<ID3D11Buffer> GSCBuffers[2] = {
		mVertexConstantBuffer,
		mGSCBuffer
	};
	context->GSSetConstantBuffers(0, 2, GSCBuffers->GetAddressOf());

	context->PSSetShader(dbgShapes->GetPS().Get(), 0, 0);
	context->PSSetConstantBuffers(0, 1, mMesh->PixelConstantBuffers.at(0).GetAddressOf());

	context->IASetInputLayout(dbgShapes->GetInputLayout().Get());
	context->IASetVertexBuffers(0, 1, mMesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mMesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->Draw(mMesh->VertexCount, 0);

	context->GSSetShader(nullptr, 0, 0);
}

void FTShape::Render(
	FoxtrotRenderer*			renderer,
	ComPtr<ID3D11VertexShader>& vertexShader,
	ComPtr<ID3D11PixelShader>&	pixelShader,
	ComPtr<ID3D11InputLayout>&	inputLayout)
{
	if (!mMesh)
		return;
	if (!mVertexConstantBuffer.Get())
		return;
	if (!mPixelConstantBuffer.Get())
		return;

	UINT stride = sizeof(DebugVertex);
	UINT offset = 0;

	UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());

	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();
	context->VSSetShader(vertexShader.Get(), 0, 0);
	context->VSSetConstantBuffers(0, 1, mMesh->VertexConstantBuffers.at(0).GetAddressOf());

	context->PSSetShader(pixelShader.Get(), 0, 0);
	context->PSSetConstantBuffers(0, 1, mMesh->PixelConstantBuffers.at(0).GetAddressOf());

	context->IASetInputLayout(inputLayout.Get());
	context->IASetVertexBuffers(0, 1, mMesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mMesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->Draw(mMesh->VertexCount, 0);
}

void FTShape::InitializeMesh(ComPtr<ID3D11Device>& device, FTDebugMeshData&& meshData)
{
	mMesh			   = DBG_NEW Mesh;
	mMesh->VertexCount = UINT(meshData.Vertices.size());

	D3D11Utils::CreateVertexBuffer(device, meshData.Vertices, mMesh->VertexBuffer);

	mMesh->VertexConstantBuffers.push_back(mVertexConstantBuffer);
	mMesh->PixelConstantBuffers.push_back(mPixelConstantBuffer);
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

void FTShape::UpdateModelMatrix(FTVector3 pos, FTVector3 rot, FTVector3 size)
{
	Matrix model =
		// Matrix::CreateScale(size.GetDXVec3()) *
		Matrix::CreateRotationY(rot.y) *
		Matrix::CreateRotationX(rot.x) *
		Matrix::CreateRotationZ(rot.z) *
		Matrix::CreateTranslation(pos.GetDXVec3());
	mVSCData.model = model.Transpose();
}

void FTShape::UpdateViewMatrix(Camera* camInst)
{
	mGSCData.view = camInst->GetViewRow().Transpose();
}

void FTShape::UpdateProjectionMatrix(Camera* camInst)
{
	mGSCData.projection = camInst->GetProjRow().Transpose();
}

void FTShape::InitializeConstantBuffer(ComPtr<ID3D11Device>& device)
{
	mVSCData.model = DirectX::SimpleMath::Matrix();

	mGSCData.view		= DirectX::SimpleMath::Matrix();
	mGSCData.projection = DirectX::SimpleMath::Matrix();

	mGSCData.size	  = Vector2::Zero;
	mPSCData.IsActive = true;

	D3D11Utils::CreateConstantBuffer(device, mVSCData, mVertexConstantBuffer);
	D3D11Utils::CreateConstantBuffer(device, mGSCData, mGSCBuffer);
	D3D11Utils::CreateConstantBuffer(device, mPSCData, mPixelConstantBuffer);
}

void FTShape::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	if (mVertexConstantBuffer)
		D3D11Utils::UpdateBuffer(context, mVSCData, mVertexConstantBuffer);
	else
		printf("ERROR : FTShape::UpdateConstantBuffers() -> Vertex Constant Buffer is null");

	if (mGSCBuffer)
		D3D11Utils::UpdateBuffer(context, mGSCData, mGSCBuffer);
	else
		Debug::LogError(__LINE__, __FILE__, "GSC Buffer is null");

	if (mPixelConstantBuffer)
		D3D11Utils::UpdateBuffer(context, mPSCData, mPixelConstantBuffer);
	else
		printf("ERROR : FTShape::UpdateConstantBuffers() -> Pixel Constant Buffer is null");
}