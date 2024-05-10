#pragma once

#include <d3d11.h>
#include <wrl.h> // ComPtr

class FoxtrotRenderer;

class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D11Device*, int, int, UINT);
	void Shutdown();
	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float* clearColor);

	ComPtr <ID3D11ShaderResourceView> GetShaderResourceView();
	ComPtr <ID3D11RenderTargetView> GetRenderTargetView();

private:
	ComPtr <ID3D11Texture2D> m_renderTargetTexture = nullptr;
	ComPtr <ID3D11RenderTargetView> m_renderTargetView = nullptr;
	ComPtr <ID3D11ShaderResourceView> m_shaderResourceView = nullptr;
};
