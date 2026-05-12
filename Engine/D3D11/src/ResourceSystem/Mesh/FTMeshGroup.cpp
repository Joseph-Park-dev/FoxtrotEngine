// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FTMeshGroup.h"

#include "ResourceSystem/Mesh/GeometryGenerator.h"
#include "ResourceSystem/Mesh/Mesh.h"
#include "ResourceSystem/Mesh/MeshConstantData.h"
#include "ResourceSystem/Material/FTMaterial.h"
#include "ResourceSystem/Shader/FTVertexShader.h"
#include "ResourceSystem/Shader/FTPixelShader.h"
#include "ResourceSystem/Shader/FTGeometryShader.h"
#include "ResourceSystem/D3D11PSO.h"
#include "Manager/ResourceManager.h"
#include "InputSystem/FTInputDevice.h"
#include "Renderer/Camera.h"
#include "Renderer/D3D11Renderer.h"
#include "Utility/D3D11Utils.h"
#include "Actor/Transform.h"
#include "ResourceSystem/Mesh/FTMeshData.h"
#include "FTDS/Dynamic/DynamicArray.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
#endif // FOXTROT_EDITOR

using Microsoft::WRL::ComPtr;

namespace D3D11
{
	using namespace Common;
	ResType FTMeshGroup::Type = ResType::MESH_GROUP;

	void FTMeshGroup::Render(D3D11Renderer* renderer, Core::Transform* transform, Camera* camInst, D3D11PSO* pso, FTMaterial* mat)
	{
	}

	void FTMeshGroup::SetSizeScale(const Math::FTVector3 scale)
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

	Common::FTDS::DynamicArray<Mesh*>* FTMeshGroup::Meshes() { return mMeshes; }
	Math::FTVector3&				 FTMeshGroup::SizeScale() { return mSizeScale; };

	// This is unused until 3D feature is implemented.
	// FTMeshGroup::FTMeshGroup(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	//	: FTResource(resDef)
	//	, mFrontDir(1)
	//	, mDirection(1)
	//	, mSizeScale(FTVector3(1.0f, 1.0f, 1.0f))
	//	, mMeshes(DBG_NEW Common::FTDS::DynamicArray<Mesh*>)
	//	, mVCData(DBG_NEW PointModelMat)
	//{
	//	Process(renderer);
	// }

	FTMeshGroup::FTMeshGroup(Common::FTResourceDef& resDef, D3D11Renderer* renderer, FTMeshData* meshData)
		: D3D11Resource(resDef)
		, mFrontDir(1)
		, mDirection(1)
		, mSizeScale(Math::FTVector3(1.0f, 1.0f, 1.0f))
		, mMeshes(DBG_NEW Common::FTDS::DynamicArray<Mesh*>)
		, mVCData(DBG_NEW PointModelMat)
	{
		if (!meshData)
		{
			Debug::LogError(__LINE__, __FILE__, "MeshData is null. If this is called while initializing FTSprite or SpineAnim, it is OK");
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

	// This is unused untill 3D feature is implemented.
	// void FTMeshGroup::Process(FoxtrotRenderer* renderer)
	//{
	//	if (this->IsProcessed())
	//		return;
	//
	//	if (this->GetRelativePath().Equal(::ChunkKey::NullVal::NULL_OBJECT))
	//		return;
	//
	//	CreateTextureSampler(renderer->GetDevice());
	//	InitializeMeshes(renderer->GetDevice(), std::move(GeometryGenerator::ReadFromFile(this->GetRelativePath())));
	//	InitializeConstantBuffers(renderer->GetDevice());
	//
	//	FTResource::Process();
	// }

	void FTMeshGroup::InitializeConstantBuffers(ComPtr<ID3D11Device>& device)
	{
		D3D11Utils::CreateConstantBuffer(device, *mVCData, mVCBuf);
	}

	HRESULT FTMeshGroup::CreateTextureSampler(ComPtr<ID3D11Device>& device)
	{
		// FTTexture sampler �����
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
		Core::Transform*			 transform,
		Camera*						 camInst,
		FTMaterial*					 mat,
		const int					 frontDir)
	{
		//// Model Transformation
		//// Front Direction will be multiplied to scale.
		//// When frontDir is minus, multiplication must be done only once as the character switches direction.

		// float linearX = transform->GetSteering()->Linear.x;
		// if (linearX < 0)
		//	mDirection = -1;
		// else if (0 < linearX)
		//	mDirection = 1;

		// FTVector3 scale		   = transform->GetWorldScale();
		// float	  scaleX	   = Math::Abs(scale.x);
		// FTVector3 scaleWithDir = FTVector3(scaleX * frontDir * mDirection, scale.y, scale.z);
		// transform->SetWorldScale(scaleWithDir);
		// Matrix modelMat = transform->GetMatrixWorld();
		// modelMat *= Matrix::CreateScale(mSizeScale.GetDXVec3());

		//// Inverse transpose matrix calculation
		//// Consider removing this part if the engine is for 2D games.
		//// Matrix invTransposeMat = modelMat.Transpose();
		//// invTransposeMat.Translation(Vector3(0.0f));
		//// invTransposeMat = invTransposeMat.Transpose().Invert();

		//// View Transformation
		// Matrix&& viewMat  = camInst->GetViewRow();
		// Vector3	 eyeWorld = Vector3::Transform(Vector3(0.0f), viewMat.Invert());

		//// Project Transformation
		// Matrix&& projMat = std::move(camInst->GetProjRow());

		// mVCData->ModelMat = modelMat.Transpose();

		// D3D11Utils::UpdateBuffer(
		//	context, *mVCData, mVCBuf);

		// if (mat)
		//	mat->UpdateBuffer(context);
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

	ComPtr<ID3D11SamplerState>& FTMeshGroup::GetSamplerState() { return mSamplerState; }
	ComPtr<ID3D11Buffer>&		FTMeshGroup::GetVCBuf() { return mVCBuf; }

	PointModelMat* FTMeshGroup::GetVCData()
	{
		return mVCData;
	}

	const Math::FTVector3& FTMeshGroup::GetSizeScale() const { return mSizeScale; }

	const int FTMeshGroup::GetDirection() const
	{
		return mDirection;
	}

	void FTMeshGroup::SetDirection(int dir)
	{
		mDirection = dir;
	}

	void FTMeshGroup::Process(D3D11Renderer* renderer, FTMeshData* meshData)
	{
		// if (this->IsProcessed())
		//	return;

		// if (this->GetRelativePath().IsEmpty())
		//	return;

		// CreateTextureSampler(renderer->GetDevice());
		// InitializeMesh(renderer->GetDevice(), meshData);
		// InitializeConstantBuffers(renderer->GetDevice());

		// FTResource::Process();
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

	void FTMeshGroup::InitializeMeshes(ComPtr<ID3D11Device>& device, Common::FTDS::DynamicArray<FTMeshData*>&& meshDataArr)
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
		Editor::UPDATE_VEC3("Scale size", mSizeScale);

		bool val = true;
		0 < mFrontDir ? val = true : val = false;

		Editor::UPDATE_BOOL("Is Facing Right", val);
		SetRightIsFront(val);
	}

#endif // FOXTROT_EDITOR
} // namespace D3D11