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

#ifdef FOXTROT_EDITOR
	#include "DebugShapes.h"
#endif // FOXTROT_EDITOR

using Matrix = DirectX::SimpleMath::Matrix;

FTShape::FTShape()
	: mMesh(nullptr)
	, mVertexConstantData()
	, mPixelConstantData()
	, mIsActive(true)
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

BasicVCData& FTShape::GetVertexConstantData() { return mVertexConstantData; }
IndexPCData& FTShape::GetPixelConstantData() { return mPixelConstantData; }
Mesh*		 FTShape::GetMesh() { return mMesh; }

void FTShape::Initialize(FoxtrotRenderer* renderer)
{
	InitializeConstantBuffer(renderer->GetDevice());
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

	context->VSSetShader(renderer->GetSolidVS().Get(), 0, 0);
	context->VSSetConstantBuffers(0, 1, mMesh->VertexConstantBuffer.GetAddressOf());

	context->PSSetShader(renderer->GetSolidPS().Get(), 0, 0);
	context->PSSetConstantBuffers(0, 1, mMesh->PixelConstantBuffer.GetAddressOf());

	context->IASetInputLayout(renderer->GetSolidInputLayout().Get());
	context->IASetVertexBuffers(0, 1, mMesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mMesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(mMesh->IndexCount, 0, 0);
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

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext());

	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();
	context->VSSetShader(vertexShader.Get(), 0, 0);
	context->VSSetConstantBuffers(0, 1, mMesh->VertexConstantBuffer.GetAddressOf());

	context->PSSetShader(pixelShader.Get(), 0, 0);
	context->PSSetConstantBuffers(0, 1, mMesh->PixelConstantBuffer.GetAddressOf());

	context->IASetInputLayout(inputLayout.Get());
	context->IASetVertexBuffers(0, 1, mMesh->VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mMesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(mMesh->IndexCount, 0, 0);
}

void FTShape::InitializeMesh(ComPtr<ID3D11Device>& device, FTMeshData&& meshData)
{
	mMesh			   = DBG_NEW Mesh;
	mMesh->IndexCount  = UINT(meshData.Indices.size());
	mMesh->VertexCount = UINT(meshData.Vertices.size());

	D3D11Utils::CreateVertexBuffer(device, meshData.Vertices, mMesh->VertexBuffer);
	D3D11Utils::CreateIndexBuffer(device, meshData.Indices, mMesh->IndexBuffer);

	mMesh->VertexConstantBuffer = mVertexConstantBuffer;
	mMesh->PixelConstantBuffer	= mPixelConstantBuffer;
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
//     mVertexConstantData.model = model.Transpose();
// }

void FTShape::UpdateConstantBufferView(Camera* camInst)
{
	mVertexConstantData.view = camInst->GetViewRow().Transpose();
}

void FTShape::UpdateConstantBufferProjection(Camera* camInst)
{
	mVertexConstantData.projection = camInst->GetProjRow().Transpose();
}

void FTShape::InitializeConstantBuffer(ComPtr<ID3D11Device>& device)
{
	mVertexConstantData.model	   = DirectX::SimpleMath::Matrix();
	mVertexConstantData.view	   = DirectX::SimpleMath::Matrix();
	mVertexConstantData.projection = DirectX::SimpleMath::Matrix();

	D3D11Utils::CreateConstantBuffer(device, mVertexConstantData, mVertexConstantBuffer);
	D3D11Utils::CreateConstantBuffer(device, mPixelConstantData, mPixelConstantBuffer);
}

void FTShape::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	if (mVertexConstantBuffer)
		D3D11Utils::UpdateBuffer(context, mVertexConstantData, mVertexConstantBuffer);
	else
		printf("ERROR : FTShape::UpdateConstantBuffers() -> Vertex Constant Buffer is null");

	if (mPixelConstantBuffer)
		D3D11Utils::UpdateBuffer(context, mPixelConstantData, mPixelConstantBuffer);
	else
		printf("ERROR : FTShape::UpdateConstantBuffers() -> Pixel Constant Buffer is null");
}