#include "D3D11Renderer.h"

void D3D11Renderer::SetViewport(FTVector2 topLeft, FTVector2 resolution)
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
	mSolidRasterizerState.Reset();
	mWireframeRasterizerState.Reset();
	mDepthStencilState.Reset();
	mDepthStencilState2D.Reset();
	mSamplerState.Reset();
	mSolidVS.Reset();
	mSolidPS.Reset();
	mSolidInputLayout.Reset();
	mTextureVS.Reset();
	mTexturePS.Reset();
	mRimTexturePS.Reset();
	mTextureInputLayout.Reset();
	mNormalVS.Reset();
	mNormalPS.Reset();
	mBlendState.Reset();
	mContext->ClearState();

#ifdef FOXTROT_EDITOR
	mViewportRenderer->Reset();
#endif // FOXTROT_EDITOR
}