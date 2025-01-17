// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Class that holds the Meshes created from a MeshData.
/// </summary>

#pragma once
#include <directxtk/SimpleMath.h>

#include "Renderer/D3D11Utils.h"
#include "ResourceSystem/MeshData.h"
#include "ResourceSystem/Mesh.h"
#include "ResourceSystem/MeshConstantData.h"
#include "ResourceSystem/FTResource.h"

class FTBasicMeshGroup : public FTResource
{
public:
	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context
	);

public:
	virtual void Initialize(
		std::vector<MeshData>& meshes,
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context
	);
	virtual void Render			 (FoxtrotRenderer* renderer, FTTexture* texture);
			void Render			 (FoxtrotRenderer* renderer, FTTexture* texture, int meshIndex);
			void Clear			 ();

public:
	//ComPtr<ID3D11Buffer>&		GetVertexConstantBuffer() { return mVertexConstantBuffer; }
	//ComPtr<ID3D11Buffer>&		GetPixelConstantBuffer()  { return mPixelConstantBuffer; }
	//BasicVertexConstantData&	GetVertexConstantData()   { return mBasicVertexConstantData; }
	//BasicPixelConstantData&		GetPixelConstantData()	  { return mBasicPixelConstantData; }

	ComPtr<ID3D11SamplerState>&	GetSamplerState()		  { return mSamplerState; }
	int							GetMeshCount()			  { return mMeshes.size(); }
	std::vector<Mesh*>&			GetMeshes()				  { return mMeshes; }

protected:
			void	InitializeConstantBuffer (ComPtr<ID3D11Device>& device);
	virtual void	InitializeMeshes		  (ComPtr<ID3D11Device>& device, std::vector<MeshData>& meshes);
	virtual HRESULT CreateTextureSampler(ComPtr<ID3D11Device>& device);

private:
	std::vector<Mesh*>			mMeshes;
	ComPtr<ID3D11SamplerState>	mSamplerState;

public:
	 FTBasicMeshGroup();
	~FTBasicMeshGroup() override;
};