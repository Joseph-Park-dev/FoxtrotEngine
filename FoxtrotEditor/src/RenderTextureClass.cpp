#include "RenderTextureClass.h"

#include <d3d11.h>
#include <wrl.h> // ComPtr
#include <functional>

#include "Renderer/FoxtrotRenderer.h"
#include "Core/TemplateFunctions.h"
#include "Physics/Physics2D.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "DebugGeometries.h"

void RenderTextureClass::InitializeTexture(ComPtr<ID3D11Device>& device, int width, int height)
{
	// Initialize Texture2D
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	device->CreateTexture2D(&textureDesc, NULL, mRenderTargetTexture.GetAddressOf());

	// Initialize RenderTargetView
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	device->CreateRenderTargetView(mRenderTargetTexture.Get(), &renderTargetViewDesc, mRenderTargetView.GetAddressOf());

	// Initialize ShaderResourceView
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(mRenderTargetTexture.Get(), &shaderResourceViewDesc, mShaderResourceView.GetAddressOf());
}

void RenderTextureClass::DrawOnTexture(ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11RenderTargetView>& renderTargetView, ComPtr<ID3D11DepthStencilView>& depthStencilView, FoxtrotRenderer* renderer)
{
	float clearColor[4] = { 0.0,0.0,0.0,1.0 };
	context->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), depthStencilView.Get());
	context->ClearRenderTargetView(mRenderTargetView.Get(), clearColor);

	//EditorLayer::GetInstance()->DisplayEditorElements(renderer);
	EditorSceneManager::GetInstance()->Render(renderer);
	EditorSceneManager::GetInstance()->EditorRender(renderer);
	DebugGeometries::GetInstance()->Render(renderer);

	context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

RenderTextureClass::RenderTextureClass()
{
}


RenderTextureClass::RenderTextureClass(const RenderTextureClass& other)
{
}


RenderTextureClass::~RenderTextureClass()
{
}