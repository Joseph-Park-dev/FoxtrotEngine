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

	void InitializeTexture(ComPtr<ID3D11Device>& device, int width, int height);
	void DrawOnTexture(
		ComPtr<ID3D11DeviceContext>& context, 
		ComPtr<ID3D11RenderTargetView>& renderTargetView, 
		ComPtr<ID3D11DepthStencilView>& depthStencilView, 
		FoxtrotRenderer* renderer
	);

public:
	ComPtr <ID3D11ShaderResourceView>& GetShaderResourceView() { return mShaderResourceView; }

private:
	ComPtr <ID3D11Texture2D>			mRenderTargetTexture;
	ComPtr <ID3D11RenderTargetView>		mRenderTargetView;
	ComPtr <ID3D11ShaderResourceView>	mShaderResourceView;

	//ComPtr<ID3D11Texture2D>			mDepthStencilBuffer;
	//ComPtr <ID3D11DepthStencilView> mDepthStencilView;
};
