#include "D3D11Renderer.h"

#include "D3D11Window.h"
#include "D3D11Utils.h"
#include "Debugging/DebugMemAlloc.h"
#include "DebugFuncs.h"
#include "Renderer/FTRectArea.h"

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

void D3D11Renderer::SetViewport(Vector2&& topLeft, Vector2&& resolution)
{
	// Set the viewport
	ZeroMemory(mViewport, sizeof(D3D11_VIEWPORT));

	mViewport->TopLeftX = topLeft.x;
	mViewport->TopLeftY = topLeft.y;
	mViewport->Width	= resolution.x;
	mViewport->Height	= resolution.y;
	// m_screenViewport.Width = static_cast<float>(m_screenHeight);
	mViewport->MinDepth = 0.0f;
	mViewport->MaxDepth = 1.0f; // Note: important for depth buffering
	mContext->RSSetViewports(1, mViewport);
}

void D3D11Renderer::SetViewport(FLOAT topLeftX, FLOAT topLeftY, FLOAT resX, FLOAT resY)
{
	// Set the viewport
	ZeroMemory(mViewport, sizeof(D3D11_VIEWPORT));

	mViewport->TopLeftX = topLeftX;
	mViewport->TopLeftY = topLeftY;
	mViewport->Width	= resX;
	mViewport->Height	= resY;
	// m_screenViewport.Width = static_cast<float>(m_screenHeight);
	mViewport->MinDepth = 0.0f;
	mViewport->MaxDepth = 1.0f; // Note: important for depth buffering
	if (mContext)
		mContext->RSSetViewports(1, mViewport);
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

D3D11Renderer::D3D11Renderer(D3D11Window* window)
	: FoxtrotRenderer()
	, mNumQualityLevels(0)
	, mViewport(DBG_NEW D3D11_VIEWPORT)
{
	Initialize(window);
}

D3D11Renderer::~D3D11Renderer()
{
	delete mViewport;
}

bool D3D11Renderer::Initialize(D3D11Window* window)
{
	UINT renderWidth  = window->GetRenderArea()->GetSize().x;
	UINT renderHeight = window->GetRenderArea()->GetSize().y;

	DX::ThrowIfFailed(
		D3D11Utils::CreateDeviceAndContext(
			window->GetHandle(), mDevice, mContext, window->GetSwapChain(), renderWidth, renderHeight, mNumQualityLevels));

	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		return false;
}
