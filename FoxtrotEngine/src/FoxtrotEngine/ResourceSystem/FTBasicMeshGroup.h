#pragma once
#include <directxtk/SimpleMath.h>

#include "FoxtrotEngine/Renderer/D3D11Utils.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"
#include "FoxtrotEngine/ResourceSystem/Mesh.h"
#include "FoxtrotEngine/ResourceSystem/MeshConstantData.h"
#include "FoxtrotEngine/ResourceSystem/FTResource.h"

class FTBasicMeshGroup : public FTResource
{
public:
	//void Initialize(
	//	const std::string& key,
	//	ComPtr<ID3D11Device>& device,
	//	ComPtr<ID3D11DeviceContext>& context
	//);s
	virtual void Initialize(
		std::vector<MeshData>& meshes,
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context
	);
	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context
	);
	void Render			 (ComPtr<ID3D11DeviceContext>& context);
	void Render			 (ComPtr<ID3D11DeviceContext>& context, int meshIndex);
	void UpdateModelWorld(DirectX::SimpleMath::Matrix& modelToWorldRow);

public:
	ComPtr<ID3D11Buffer>&		GetVertexConstantBuffer() { return mVertexConstantBuffer; }
	ComPtr<ID3D11Buffer>&		GetPixelConstantBuffer() { return mPixelConstantBuffer; }
	BasicVertexConstantData&	GetVertexConstantData() { return mBasicVertexConstantData; }
	BasicPixelConstantData&		GetPixelConstantData()	{ return mBasicPixelConstantData; }
	DirectX::SimpleMath::Matrix GetModelWorldRow()		{ return mModelWorldRow; }
	FTTexture*					GetTexture()			{ return mTexture; }
	int							GetMeshCount()			{ return mMeshes.size(); }

	void					 SetTexture(std::string key);

private:
	DirectX::SimpleMath::Matrix mModelWorldRow	 = DirectX::SimpleMath::Matrix();
	DirectX::SimpleMath::Matrix mInvTransposeRow = DirectX::SimpleMath::Matrix();

	BasicVertexConstantData mBasicVertexConstantData;
	BasicPixelConstantData	mBasicPixelConstantData;

private:
	std::vector<Mesh*>			mMeshes;
	FTTexture*					mTexture;

	ComPtr<ID3D11VertexShader>	mBasicVertexShader;
	ComPtr<ID3D11PixelShader>	mBasicPixelShader;
	ComPtr<ID3D11InputLayout>	mBasicInputLayout;

	ComPtr<ID3D11SamplerState>	mSamplerState;

	ComPtr<ID3D11Buffer>		mVertexConstantBuffer;
	ComPtr<ID3D11Buffer>		mPixelConstantBuffer;

private:
	HRESULT CreateTextureSampler(ComPtr<ID3D11Device>& device);

public:
	FTBasicMeshGroup();
	~FTBasicMeshGroup() override;
};