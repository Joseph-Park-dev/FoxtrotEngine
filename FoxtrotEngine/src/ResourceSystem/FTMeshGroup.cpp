// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/FTMeshGroup.h"

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

void FTMeshGroup::Render(
	FoxtrotRenderer* renderer,
	Transform*		 transform,
	Camera*			 camInst,
	FTTexture*		 tex,
	FTVertexShader*	 vs,
	FTPixelShader*	 ps,
	FTMaterial*		 mat)
{
	// This enables the resource reusable throughout the Component instances.
	UpdateConstantBuffers(renderer->GetDevice(), renderer->GetContext(), transform, camInst, mat, mFrontDir);

	if (!vs || !ps) // Vertex Shader is always required when drawing.
		return;

	UINT						 stride	 = sizeof(Vertex);
	UINT						 offset	 = 0;
	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	mMeshes->IterateArray([&](Mesh* mesh) {
		context->VSSetConstantBuffers(0, 1, mVCBuf.GetAddressOf());

		if (tex)
		{
			std::vector<ID3D11ShaderResourceView*> resViews;
			resViews.push_back(tex->GetSRV().Get());
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

void FTMeshGroup::SetSizeScale(const FTVector3 scale)
{
	mSizeScale = scale;
}

const int FTMeshGroup::GetFrontDir() const
{
	return mFrontDir;
}

void FTMeshGroup::SetRightIsFront(bool val)
{
	val ? mFrontDir = 1 : mFrontDir = -1;
}

FTMeshGroup::FTMeshGroup(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTResource(resDef)
	, mFrontDir(1)
	, mDirection(1)
	, mSizeScale(FTVector3(1.0f, 1.0f, 1.0f))
	, mMeshes(DBG_NEW FTDS::DynamicArray<Mesh*>)
	, mVCData(DBG_NEW PointMVP)
{
	Process(renderer);
}

FTMeshGroup::FTMeshGroup(FTResourceDef& resDef, FoxtrotRenderer* renderer, FTMeshData* meshData)
	: FTResource(resDef)
	, mFrontDir(1)
	, mDirection(1)
	, mSizeScale(FTVector3(1.0f, 1.0f, 1.0f))
	, mMeshes(DBG_NEW FTDS::DynamicArray<Mesh*>)
	, mVCData(DBG_NEW PointMVP)
{
	if (!meshData)
	{
		Debug::LogError(__LINE__, __FILE__, "MeshData is null. If this is called while initializing FTSpineAnimation, it is OK");
		return;
	}
	Process(renderer, meshData);
}

FTMeshGroup::~FTMeshGroup()
{
	Clear();
	delete mMeshes;
	delete mVCData;
}

void FTMeshGroup::Process(FoxtrotRenderer* renderer)
{
	if (this->IsProcessed())
		return;

	if (this->GetRelativePath().Equal(ChunkKey::NullVal::NULL_OBJECT))
		return;

	Initialize(
		GeometryGenerator::ReadFromFile(this->GetRelativePath()), renderer->GetDevice(), renderer->GetContext());

	FTResource::Process();
}

void FTMeshGroup::Initialize(
	FTDS::DynamicArray<FTMeshData*>&& meshes,
	ComPtr<ID3D11Device>&			  device,
	ComPtr<ID3D11DeviceContext>&	  context)
{
	CreateTextureSampler(device);
	InitializeMeshes(device, std::move(meshes));
	InitializeConstantBuffers(device);
}

void FTMeshGroup::InitializeConstantBuffers(ComPtr<ID3D11Device>& device)
{
	D3D11Utils::CreateConstantBuffer(device, *mVCData, mVCBuf);
}

HRESULT FTMeshGroup::CreateTextureSampler(ComPtr<ID3D11Device>& device)
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

void FTMeshGroup::UpdateConstantBuffers(
	ComPtr<ID3D11Device>&		 device,
	ComPtr<ID3D11DeviceContext>& context,
	Transform*					 transform,
	Camera*						 camInst,
	FTMaterial*					 mat,
	const int					 frontDir)
{
	// Model Transformation
	// Front Direction will be multiplied to scale.
	// When frontDir is minus, multiplication must be done only once as the character switches direction.

	float linearX = transform->GetSteering()->Linear.x;
	if (linearX < 0)
		mDirection = -1;
	else if (0 < linearX)
		mDirection = 1;

	FTVector3 scale		   = transform->GetWorldScale();
	float	  scaleX	   = Math::Abs(scale.x);
	FTVector3 scaleWithDir = FTVector3(scaleX * frontDir * mDirection, scale.y, scale.z);
	transform->SetWorldScale(scaleWithDir);
	Matrix modelMat = transform->GetMatrixWorld();
	modelMat *= Matrix::CreateScale(mSizeScale.GetDXVec3());

	// Inverse transpose matrix calculation
	// Consider removing this part if the engine is for 2D games.
	//Matrix invTransposeMat = modelMat.Transpose();
	//invTransposeMat.Translation(Vector3(0.0f));
	//invTransposeMat = invTransposeMat.Transpose().Invert();

	// View Transformation
	Matrix&& viewMat  = camInst->GetViewRow();
	Vector3	 eyeWorld = Vector3::Transform(Vector3(0.0f), viewMat.Invert());

	// Project Transformation
	Matrix&& projMat = std::move(camInst->GetProjRow());

	mVCData->ModelMat = modelMat.Transpose();
	mVCData->ViewMat  = viewMat.Transpose();
	mVCData->ProjMat  = projMat.Transpose();

	D3D11Utils::UpdateBuffer(
		context, *mVCData, mVCBuf);

	if (mat)
		mat->UpdateBuffer(context);
}

void FTMeshGroup::Clear()
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

FTDS::DynamicArray<Mesh*>*	FTMeshGroup::Meshes() { return mMeshes; };
ComPtr<ID3D11SamplerState>& FTMeshGroup::GetSamplerState() { return mSamplerState; }
ComPtr<ID3D11Buffer>&		FTMeshGroup::GetVCBuf() { return mVCBuf; }

const FTVector3& FTMeshGroup::GetSizeScale() const { return mSizeScale; }

const int FTMeshGroup::GetDirection() const
{
	return mDirection;
}

void FTMeshGroup::SetDirection(int dir)
{
	mDirection = dir;
}

void FTMeshGroup::Process(FoxtrotRenderer* renderer, FTMeshData* meshData)
{
	if (this->IsProcessed())
		return;

	if (this->GetRelativePath().IsEmpty())
		return;

	Initialize(meshData, renderer->GetDevice(), renderer->GetContext());

	FTResource::Process();
}

void FTMeshGroup::Initialize(
	FTMeshData*					 mesh,
	ComPtr<ID3D11Device>&		 device,
	ComPtr<ID3D11DeviceContext>& context)
{
	CreateTextureSampler(device);
	InitializeMesh(device, mesh);
	InitializeConstantBuffers(device);
}

void FTMeshGroup::InitializeMesh(ComPtr<ID3D11Device>& device, FTMeshData* meshData)
{
	Mesh* newMesh		 = DBG_NEW Mesh;
	newMesh->VertexCount = UINT(meshData->Vertices.GetSize());
	newMesh->IndexCount	 = UINT(meshData->Indices.GetSize());

	D3D11Utils::CreateVertexBuffer(device, meshData->Vertices, newMesh->VertexBuffer);
	D3D11Utils::CreateIndexBuffer(device, meshData->Indices, newMesh->IndexBuffer);

	mMeshes->PushBack(newMesh);
}

void FTMeshGroup::InitializeMeshes(ComPtr<ID3D11Device>& device, FTDS::DynamicArray<FTMeshData*>&& meshDataArr)
{
	if (0 < meshDataArr.GetSize())
		Clear();

	mMeshes->Reserve(meshDataArr.GetSize());

	meshDataArr.IterateArray([&](FTMeshData* meshData) {
		this->InitializeMesh(device, meshData);
		delete meshData;
	});
}

#ifdef FOXTROT_EDITOR
void FTMeshGroup::UpdateUI()
{
	CommandHistory::GetInstance()->UpdateVector3Value("Scale size", mSizeScale);

	bool val = true;
	0 < mFrontDir ? val = true : val = false;

	CommandHistory::GetInstance()->UpdateBoolValue("Is Facing Right", val);
	SetRightIsFront(val);
}

#endif // FOXTROT_EDITOR