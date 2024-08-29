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
	//);
	void Initialize(
		const std::vector<MeshData> meshes,
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context
	);
	void UpdateConstantBuffers(
		ComPtr<ID3D11Device>& device,
		ComPtr<ID3D11DeviceContext>& context
	);
	void Render			 (ComPtr<ID3D11DeviceContext>& context);
	void UpdateModelWorld(DirectX::SimpleMath::Matrix& modelToWorldRow);

public:
	BasicVertexConstantData&	GetVertexConstantData() { return mBasicVertexConstantData; }
	BasicPixelConstantData&		GetPixelConstantData()	{ return mBasicPixelConstantData; }
	DirectX::SimpleMath::Matrix GetModelWorldRow()		{ return mModelWorldRow; }
	FTTexture*					GetTexture()			{ return mMeshes[0]->texture; }
	FTTexture*					GetTexture(int index)	{ return mMeshes[index]->texture; }

	void					 SetTexture(std::string key);
	void					 SetTexture(std::string key, int index);

private:
	DirectX::SimpleMath::Matrix mModelWorldRow	 = DirectX::SimpleMath::Matrix();
	DirectX::SimpleMath::Matrix mInvTransposeRow = DirectX::SimpleMath::Matrix();

	// ExampleApp::Update()에서 접근
	BasicVertexConstantData mBasicVertexConstantData;
	BasicPixelConstantData	mBasicPixelConstantData;

private:
	// 메쉬 그리기
	std::vector<Mesh*>			mMeshes;

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

