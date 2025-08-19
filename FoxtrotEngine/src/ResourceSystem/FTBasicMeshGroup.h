// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Class that holds the Meshes created from a FTMeshData.
/// </summary>

#pragma once
#include <directxtk/SimpleMath.h>

#include "Renderer/D3D11Utils.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/MeshConstantData.h"
#include "ResourceSystem/FTResource.h"

class FTMaterial;
class FTVertexShader;
class FTMeshData;

// Number of pixel shader constant buffers
constexpr size_t MAX_PS_BUFFER_COUNT = 20;

class FTBasicMeshGroup : public FTResource
{
public:
	void CalcVCData(Transform* transform, Camera* camInst);

	virtual void UpdateConstantBuffers(
		ComPtr<ID3D11Device>&			 device,
		ComPtr<ID3D11DeviceContext>&	 context,
		FTMaterial* mat);

public:
	void Initialize(
		FTMeshData&&				 meshData,
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context);

	virtual void Initialize(
		FTDS::DynamicArray<FTMeshData>&& meshData,
		ComPtr<ID3D11Device>&			 device,
		ComPtr<ID3D11DeviceContext>&	 context);

	virtual void Render(
		FoxtrotRenderer* renderer,
		FTTexture*		 tex,
		FTVertexShader*	 vs,
		FTPixelShader*	 ps,
		FTMaterial*		 mat);
	virtual void Render(
		int				 meshIndex,
		FoxtrotRenderer* renderer,
		FTTexture*		 tex,
		FTVertexShader*	 vs,
		FTPixelShader*	 ps,
		FTMaterial*		 mat);

	void Clear();

public:
	ComPtr<ID3D11SamplerState>& GetSamplerState();
	BasicVCData&				GetVCData();
	ComPtr<ID3D11Buffer>&		GetVCBuf();

	FTDS::DynamicArray<Mesh*>* Meshes();

public:
	FTBasicMeshGroup();
	virtual ~FTBasicMeshGroup();

protected:
	virtual HRESULT CreateTextureSampler(ComPtr<ID3D11Device>& device);
	virtual void	InitializeMesh(ComPtr<ID3D11Device>& device, FTMeshData&& meshData);
	virtual void	InitializeMeshes(ComPtr<ID3D11Device>& device, FTDS::DynamicArray<FTMeshData>&& meshDataArr);
	virtual void	InitializeConstantBuffers(ComPtr<ID3D11Device>& device);

private:
	int mDirection;

	FTDS::DynamicArray<Mesh*>* mMeshes;
	ComPtr<ID3D11SamplerState> mSamplerState;

	ComPtr<ID3D11Buffer> mVertexConstBuffer;
	BasicVCData			 mVertexConstData;

private:
	void CalcModelMat(Matrix& matrix, Transform* transform);

public:
	virtual void Process(FTCore* coreInst) override;
};

namespace ChunkKey
{
	namespace FTMeshGroup
	{
		constexpr const char* MESH_KEY	  = "Mesh Key";
		constexpr const char* TEXTURE_KEY = "Texture Key";
		constexpr const char* SHADER_KEY  = "Shader Key";
		constexpr const char* VS_KEY	  = "Vertex Shader Key";
		constexpr const char* PS_KEY	  = "Pixel Shader Key";
		constexpr const char* MAT_KEY	  = "Material Key";

		constexpr const char* DRAW_TEXTURE = "Draw Texture";
		constexpr const char* DRAW_NORMALS = "Draw Normals";
	} // namespace FTMeshGroup
} // namespace ChunkKey