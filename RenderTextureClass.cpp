#include "RenderTextureClass.h"
#include "FoxtrotRenderer.h"


RenderTextureClass::RenderTextureClass()
{
}


RenderTextureClass::RenderTextureClass(const RenderTextureClass& other)
{
}


RenderTextureClass::~RenderTextureClass()
{
}


bool RenderTextureClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight, UINT numQualityLevels)
{
	// 렌터 타겟 텍스처 설명을 초기화합니다
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// 렌더 타겟 텍스처 설명을 설정합니다
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	if (numQualityLevels > 0) {
		textureDesc.SampleDesc.Count = 4; // how many multisamples
		textureDesc.SampleDesc.Quality = numQualityLevels - 1;
	}
	else {
		textureDesc.SampleDesc.Count = 1; // how many multisamples
		textureDesc.SampleDesc.Quality = 0;
	}
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// 렌더 타겟 텍스처를 만듭니다
	HRESULT result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더 타겟 뷰의 설명을 설정합니다
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// 렌더 타겟 뷰를 생성한다
	result = device->CreateRenderTargetView(m_renderTargetTexture.Get(), &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// 셰이더 리소스 뷰의 설명을 설정합니다
	/*D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;*/

	// 셰이더 리소스 뷰를 만듭니다
	result = device->CreateShaderResourceView(m_renderTargetTexture.Get(), NULL, &m_shaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void RenderTextureClass::Shutdown()
{
	if (m_shaderResourceView)
	{
		m_shaderResourceView.Reset();
	}

	if (m_renderTargetView)
	{
		m_renderTargetView.Reset();
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture.Reset();
	}
}

void RenderTextureClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// 렌더링 대상 뷰와 깊이 스텐실버퍼를 출력 렌더 파이프 라인에 바인딩합니다
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);
}


void RenderTextureClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, float* clearColor)
{
	// 백 버퍼를 지운다
	deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);

	// 깊이 버퍼를 지운다
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

ComPtr <ID3D11ShaderResourceView> RenderTextureClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}

ComPtr <ID3D11RenderTargetView> RenderTextureClass::GetRenderTargetView()
{
	return m_renderTargetView;
}
