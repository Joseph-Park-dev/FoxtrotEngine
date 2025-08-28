// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/FTBasicMeshGroup.h"

#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "ResourceSystem/FTShaders/FTVertexShader.h"
#include "ResourceSystem/FTShaders/FTPixelShader.h"
#include "Managers/ResourceManager.h"
#include "InputSystem/FTInputDevice.h"
#include "Renderer/Camera.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Actors/Transform.h"
#include <ResourceSystem/FTMeshData.h>
#include <Dynamic/DynamicArray.h>

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
	#include "EditorResourceManager.h"
#endif // FOXTROT_EDITOR

using Matrix = DirectX::SimpleMath::Matrix;

void FTBasicMeshGroup::Initialize(
	FTMeshData*					 mesh,
	ComPtr<ID3D11Device>&		 device,
	ComPtr<ID3D11DeviceContext>& context)
{
	CreateTextureSampler(device);
	InitializeMesh(device, mesh);
	InitializeConstantBuffers(device);
}

void FTBasicMeshGroup::Initialize(
	FTDS::DynamicArray<FTMeshData*>&& meshes,
	ComPtr<ID3D11Device>&			  device,
	ComPtr<ID3D11DeviceContext>&	  context)
{
	CreateTextureSampler(device);
	InitializeMeshes(device, std::move(meshes));
	InitializeConstantBuffers(device);
}

void FTBasicMeshGroup::Render(
	FoxtrotRenderer* renderer,
	FTTexture*		 tex,
	FTVertexShader*	 vs,
	FTPixelShader*	 ps,
	FTMaterial*		 mat)
{
	if (!vs || !ps) // Vertex Shader is always required when drawing.
		return;

	UINT						 stride	 = sizeof(Vertex);
	UINT						 offset	 = 0;
	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	mMeshes->IterateArray([&](Mesh* mesh) {
		context->VSSetConstantBuffers(0, 1, mVertexConstBuffer.GetAddressOf());

		if (tex)
		{
			std::vector<ID3D11ShaderResourceView*> resViews;
			resViews.push_back(tex->GetResourceView().Get());
			context->PSSetShaderResources(0, (UINT)resViews.size(), resViews.data());
		}

		context->VSSetShader(vs->GetShader().Get(), 0, 0);
		context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
		context->PSSetShader(ps->GetShader().Get(), 0, 0);

		if (mat)
			context->PSSetConstantBuffers(0, 1, mat->GetPCBuf().GetAddressOf());

		context->IASetInputLayout(vs->GetInputLayout().Get());
		context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(mesh->IndexCount, 0, 0);
	});
}

void FTBasicMeshGroup::Render(
	int				 meshIndex,
	FoxtrotRenderer* renderer,
	FTTexture*		 tex,
	FTVertexShader*	 vs,
	FTPixelShader*	 ps,
	FTMaterial*		 mat)
{
	if (!vs || !ps) // Vertex Shader is always required when drawing.
		return;

	UINT						 stride	 = sizeof(Vertex);
	UINT						 offset	 = 0;
	Mesh*						 mesh	 = mMeshes->At(meshIndex);
	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	if (mesh)
	{
		context->VSSetConstantBuffers(
			0, 1, mVertexConstBuffer.GetAddressOf());

		if (tex)
		{
			std::vector<ID3D11ShaderResourceView*> resViews;
			resViews.push_back(tex->GetResourceView().Get());
			context->PSSetShaderResources(0, (UINT)resViews.size(), resViews.data());
		}

		context->VSSetShader(vs->GetShader().Get(), 0, 0);
		context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
		context->PSSetShader(ps->GetShader().Get(), 0, 0);

		if (mat)
			context->PSSetConstantBuffers(0, 1, mat->GetPCBuf().GetAddressOf());

		context->IASetInputLayout(vs->GetInputLayout().Get());
		context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(mesh->IndexCount, 0, 0);
	}
}

void FTBasicMeshGroup::CalcVCData(Transform* transform, Camera* camInst)
{
	// Model Transformation
	Matrix modelMat = Matrix();
	CalcModelMat(modelMat, transform);
	Matrix invTransposeMat = modelMat.Transpose();
	invTransposeMat.Translation(Vector3(0.0f));
	invTransposeMat = invTransposeMat.Transpose().Invert();

	// View Transformation
	Matrix&& viewMat  = camInst->GetViewRow();
	Vector3	 eyeWorld = Vector3::Transform(Vector3(0.0f), viewMat.Invert());

	// Project Transformation
	Matrix&& projMat = std::move(camInst->GetProjRow());

	mVertexConstData.model		  = modelMat.Transpose();
	mVertexConstData.view		  = viewMat.Transpose();
	mVertexConstData.projection	  = projMat.Transpose();
	mVertexConstData.invTranspose = std::move(invTransposeMat);
}

void FTBasicMeshGroup::UpdateConstantBuffers(
	ComPtr<ID3D11Device>&		 device,
	ComPtr<ID3D11DeviceContext>& context,
	FTMaterial*					 mat)
{
	D3D11Utils::UpdateBuffer(
		context, mVertexConstData, mVertexConstBuffer);

	if (mat)
		mat->UpdateBuffer(context);
}

void FTBasicMeshGroup::Clear()
{
	mMeshes->IterateArray([&](Mesh* mesh) {
		if (mesh)
		{
			delete mesh;
			mesh = nullptr;
		}
	});
	mMeshes->Clear();
}

ComPtr<ID3D11SamplerState>& FTBasicMeshGroup::GetSamplerState() { return mSamplerState; }
BasicVCData&				FTBasicMeshGroup::GetVCData() { return mVertexConstData; }
ComPtr<ID3D11Buffer>&		FTBasicMeshGroup::GetVCBuf() { return mVertexConstBuffer; }
FTDS::DynamicArray<Mesh*>*	FTBasicMeshGroup::Meshes() { return mMeshes; };

void FTBasicMeshGroup::InitializeMeshes(ComPtr<ID3D11Device>& device, FTDS::DynamicArray<FTMeshData*>&& meshDataArr)
{
	if (0 < meshDataArr.GetSize())
		Clear();

	mMeshes->Reserve(meshDataArr.GetSize());

	meshDataArr.IterateArray([&](FTMeshData* meshData) {
		this->InitializeMesh(device, meshData);
		delete meshData;
	});
}

void FTBasicMeshGroup::InitializeMesh(ComPtr<ID3D11Device>& device, FTMeshData* meshData)
{
	Mesh* newMesh		 = DBG_NEW Mesh;
	newMesh->VertexCount = UINT(meshData->Vertices.GetSize());
	newMesh->IndexCount	 = UINT(meshData->Indices.GetSize());

	D3D11Utils::CreateVertexBuffer(device, meshData->Vertices, newMesh->VertexBuffer);
	D3D11Utils::CreateIndexBuffer(device, meshData->Indices, newMesh->IndexBuffer);

	mMeshes->PushBack(newMesh);
}

void FTBasicMeshGroup::InitializeConstantBuffers(ComPtr<ID3D11Device>& device)
{
	D3D11Utils::CreateConstantBuffer(device, mVertexConstData, mVertexConstBuffer);
}

HRESULT FTBasicMeshGroup::CreateTextureSampler(ComPtr<ID3D11Device>& device)
{
	// FTTexture sampler ¸¸µé±â
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU		= D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV		= D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW		= D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD			= 0;
	sampDesc.MaxLOD			= D3D11_FLOAT32_MAX;

	// Create the Sample State
	return device->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());
}

FTBasicMeshGroup::FTBasicMeshGroup()
	: mDirection(1)
	, mMeshes(DBG_NEW FTDS::DynamicArray<Mesh*>)
{
}

FTBasicMeshGroup::~FTBasicMeshGroup()
{
	Clear();
	delete mMeshes;
}

void FTBasicMeshGroup::CalcModelMat(Matrix& matrix, Transform* transform)
{
	mDirection += transform->GetSteering()->Linear.x;
	Math::Clamp(mDirection, -1, 1);

	FTVector3 scale		   = transform->GetWorldScale();
	FTVector3 scaleWithDir = FTVector3(scale.x * mDirection, scale.y, scale.z);
	transform->SetLocalScale(scaleWithDir);
	matrix = transform->GetMatrixWorld();
}

void FTBasicMeshGroup::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	if (this->RelativePath().IsEmpty())
		return;

	FoxtrotRenderer* rnd = coreInst->GetGameRenderer();
	Initialize(GeometryGenerator::ReadFromFile(RelativePath()), rnd->GetDevice(), rnd->GetContext());

	this->SetIsProcessed(true);
}