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
	//void UpdateModelWorld(DirectX::SimpleMath::Matrix& modelToWorldRow);
	void ResetTexture();

public:
	virtual void Initialize(
		std::vector<MeshData>& meshes,
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context
	);
	virtual void Update			 (float deltaTime) {};
	virtual	void Render			 (FoxtrotRenderer* renderer);
			void Render			 (FoxtrotRenderer* renderer, int meshIndex);

public:
	ComPtr<ID3D11Buffer>&		GetVertexConstantBuffer() { return mVertexConstantBuffer; }
	ComPtr<ID3D11Buffer>&		GetPixelConstantBuffer()  { return mPixelConstantBuffer; }
	BasicVertexConstantData&	GetVertexConstantData()   { return mBasicVertexConstantData; }
	BasicPixelConstantData&		GetPixelConstantData()	  { return mBasicPixelConstantData; }

	ComPtr<ID3D11SamplerState>&	GetSamplerState()	{ return mSamplerState; }

	FTTexture*					GetTexture()			  { return mTexture; }
	int							GetMeshCount()			  { return mMeshes.size(); }
	std::vector<Mesh*>&			GetMeshes()				  { return mMeshes; }

	void						SetTexture(UINT key);

protected:
			void	InitializeConstantBuffer (ComPtr<ID3D11Device>& device);
	virtual void	InitializeMeshes		  (ComPtr<ID3D11Device>& device, std::vector<MeshData>& meshes);
	virtual HRESULT CreateTextureSampler(ComPtr<ID3D11Device>& device);

private:
	//DirectX::SimpleMath::Matrix mModelWorldRow	 = DirectX::SimpleMath::Matrix();
	//DirectX::SimpleMath::Matrix mInvTransposeRow = DirectX::SimpleMath::Matrix();

	BasicVertexConstantData mBasicVertexConstantData;
	BasicPixelConstantData	mBasicPixelConstantData;

private:
	std::vector<Mesh*>			mMeshes;
	FTTexture*					mTexture;

	ComPtr<ID3D11SamplerState>	mSamplerState;

	ComPtr<ID3D11Buffer>		mVertexConstantBuffer;
	ComPtr<ID3D11Buffer>		mPixelConstantBuffer;

public:
	 FTBasicMeshGroup();
	~FTBasicMeshGroup() override;

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI() override {};
#endif // FOXTROT_EDITOR
};