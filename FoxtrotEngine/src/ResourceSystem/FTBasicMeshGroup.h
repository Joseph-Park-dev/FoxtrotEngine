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
#include "ResourceSystem/FTMeshData.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/MeshConstantData.h"
#include "ResourceSystem/FTResource.h"

class FTMaterial;

class FTBasicMeshGroup : public FTResource
{
public:
	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context);

public:
	virtual void Initialize(
		std::vector<FTMeshData>&	 meshes,
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context);
	virtual void Render(FoxtrotRenderer* renderer, FTTexture* texture);
	void		 Render(FoxtrotRenderer* renderer, FTTexture* texture, int meshIndex);
	void		 Clear();

public:
	ComPtr<ID3D11SamplerState>& GetSamplerState();
	size_t						GetMeshCount();
	std::vector<Mesh*>&			GetMeshes();

	BasicVCData& GetVCData();
	std::vector<FTMaterial*>& Materials();
	void SetMaterials(std::vector<UINT>& matKeys, ComPtr<ID3D11Device>& device);

	bool GetDrawNormal();

	void SetDrawNormal(bool drawNormal);

public:
	FTBasicMeshGroup();
	virtual ~FTBasicMeshGroup() override;

protected:
	virtual HRESULT CreateTextureSampler(ComPtr<ID3D11Device>& device);
	virtual void	InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<FTMeshData>& meshes);
	void			InitializeConstantBuffers(ComPtr<ID3D11Device>& device);

private:
	std::vector<Mesh*>		   mMeshes;
	ComPtr<ID3D11SamplerState> mSamplerState;

	ComPtr<ID3D11Buffer>	 mVertexConstBuffer;
	//ComPtr<ID3D11Buffer>	 mPixelConstBuffer;
	BasicVCData				 mVertexConstData;
	std::vector<FTMaterial*> mMaterials;

	Mesh*		 mNormalLines;
	NormalVCData mNormalVertexConstData;

	bool mDrawNormal;

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI() override;

private:
	bool mValModified;
#endif
};

namespace ChunkKey
{
	constexpr const char* FTMESHGROUP_DRAW_TEXTURE = "Draw Texture";
	constexpr const char* FTMESHGROUP_DRAW_NORMALS = "Draw Normals";
} // namespace ChunkKey