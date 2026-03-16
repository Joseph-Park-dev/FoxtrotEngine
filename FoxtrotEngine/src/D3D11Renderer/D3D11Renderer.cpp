#include "D3D11Renderer.h"

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

void D3D11Renderer::SetViewport(Vector2&& topLeft, Vector2&& resolution)
{
	// Set the viewport
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));

	mScreenViewport.TopLeftX = topLeft.x;
	mScreenViewport.TopLeftY = topLeft.y;
	mScreenViewport.Width	 = resolution.x;
	mScreenViewport.Height	 = resolution.y;
	// m_screenViewport.Width = static_cast<float>(m_screenHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f; // Note: important for depth buffering
	mContext->RSSetViewports(1, &mScreenViewport);
}

void D3D11Renderer::SetViewport(FLOAT topLeftX, FLOAT topLeftY, FLOAT resX, FLOAT resY)
{
	// Set the viewport
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));

	mScreenViewport.TopLeftX = topLeftX;
	mScreenViewport.TopLeftY = topLeftY;
	mScreenViewport.Width	 = resX;
	mScreenViewport.Height	 = resY;
	// m_screenViewport.Width = static_cast<float>(m_screenHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f; // Note: important for depth buffering
	if (mContext)
		mContext->RSSetViewports(1, &mScreenViewport);
}

void D3D11Renderer::Reset()
{
	mContext->ClearState();

#ifdef FOXTROT_EDITOR
	mViewportRenderer->Reset();
#endif // FOXTROT_EDITOR
}

ComPtr<ID3D11Device>&		 D3D11Renderer::GetDevice() { return mDevice; }
ComPtr<ID3D11DeviceContext>& D3D11Renderer::GetContext() { return mContext; }
const unsigned int			 D3D11Renderer::GetNumQualityLevels() const { return mNumQualityLevels; }

D3D11Renderer::D3D11Renderer()
	: mNumQualityLevels(0)
{
}
