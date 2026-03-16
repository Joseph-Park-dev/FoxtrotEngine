#pragma once
#include "Core/Renderer/FoxtrotRenderer.h"

#include <wrl.h>
#include <windows.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#define WIN32_LEAN_AND_MEAN

class D3D11Renderer :
	public FoxtrotRenderer
{
public:
	void SetViewport(
		DirectX::SimpleMath::Vector2&& topLeft,
		DirectX::SimpleMath::Vector2&& resolution);
	void SetViewport(FLOAT topLeftX, FLOAT topLeftY, FLOAT resX, FLOAT resY);
	void Reset();

public:
	// D3D11 interfaces (Getters).
	Microsoft::WRL::ComPtr<ID3D11Device>&		 GetDevice();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext();
	const unsigned int							 GetNumQualityLevels() const;

public:
	D3D11Renderer();

private:
	Microsoft::WRL::ComPtr<ID3D11Device>		mDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
	D3D11_VIEWPORT								mScreenViewport;
	UINT										mNumQualityLevels;
};
