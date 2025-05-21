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
	virtual void CalcVCData(Transform* transform, Camera* camInst);

	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context);

public:
	virtual void Initialize(
		std::vector<FTMeshData>&&	 meshes,
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context);

	virtual void Render(FoxtrotRenderer* renderer);
	virtual void Render(FoxtrotRenderer* renderer, int meshIndex);

	void Clear();

public:
	ComPtr<ID3D11SamplerState>& GetSamplerState();
	size_t						GetMeshCount();
	FTDS::String&				GetTexKey();
	FTTexture*					GetTexture() const;
	BasicVCData&				GetVCData();
	bool						GetDrawNormal();

	void		 SetMaterials(std::vector<FTDS::String>& matKeys, ComPtr<ID3D11Device>& device);
	virtual void SetTexture();
	virtual void SetTexture(FTDS::String& texKey);
	void		 SetTexture(FTTexture* tex);
	void		 SetNormalLines(Mesh* normalLines);
	void		 SetDrawNormal(bool drawNormal);

	std::vector<Mesh*>&		  Meshes();
	std::vector<FTMaterial*>& Materials();
	Mesh*					  NormalLines();

public:
	FTBasicMeshGroup();
	FTBasicMeshGroup(FTMeshData meshData, FoxtrotRenderer* renderer);
	virtual ~FTBasicMeshGroup();

protected:
	virtual HRESULT CreateTextureSampler(ComPtr<ID3D11Device>& device);
	virtual void	InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<FTMeshData>& meshes);
	void			InitializeConstantBuffers(ComPtr<ID3D11Device>& device);

protected:
	void SetTexKey(FTDS::String& texKey);

	ComPtr<ID3D11VertexShader>& GetVertexShader();
	ComPtr<ID3D11PixelShader>&	GetPixelShader();

	void SetVertexShader(ComPtr<ID3D11VertexShader>& vs);
	void SetPixelShader(ComPtr<ID3D11PixelShader>& ps);

	// Since the texture type is diverged into FTTexture & FTCUBEMAP_TEXTURE,
	// the TEXTURE_MAP needs to be specified.
	template <typename TEXTURE_MAP>
	void SetTexture(FTDS::String& key, TEXTURE_MAP& map)
	{
		mTexKey	 = key;
		mTexture = map.at(key);
	}

private:
	FTDS::String mTexKey;

	std::vector<Mesh*>		   mMeshes;
	FTTexture*				   mTexture;
	ComPtr<ID3D11SamplerState> mSamplerState;
	ComPtr<ID3D11VertexShader> mVS;
	ComPtr<ID3D11PixelShader>  mPS;

	ComPtr<ID3D11Buffer> mVertexConstBuffer;
	// ComPtr<ID3D11Buffer>	 mPixelConstBuffer;
	BasicVCData				 mVertexConstData;
	std::vector<FTMaterial*> mMaterials;

	Mesh*		 mNormalLines;
	NormalVCData mNormalVertexConstData;
	bool		 mDrawNormal;

private:
	void CalcModelMat(Matrix& matrix, Transform* transform);

public:
	virtual void SaveProperties(std::ofstream& ofs) override;
	virtual void LoadProperties(std::ifstream& ifs) override;
	virtual void Process(FTCore* coreInst) override;

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI();

private:
	bool mValModified;
#endif
};

namespace ChunkKey
{
	namespace FTMeshGroup
	{
		constexpr const char* TEXTURE_KEY = "Texture Key";

		constexpr const char* DRAW_TEXTURE = "Draw Texture";
		constexpr const char* DRAW_NORMALS = "Draw Normals";
	} // namespace FTMeshGroup
} // namespace ChunkKey