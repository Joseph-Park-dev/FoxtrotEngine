#include <stdexcept>
#include "Renderer/D3D11Renderer.h"

#include "Renderer/D3D11Window.h"
#include "Utility/D3D11Utils.h"
#include "Debugging/DebugMemAlloc.h"
#include "DebugFuncs.h"
#include "Renderer/FTRectArea.h"

namespace D3D11
{
	using namespace Core;
	using namespace Math;
	using namespace Microsoft::WRL;

	void D3D11Renderer::Reset()
	{
		if (mContext) { mContext->ClearState(); mContext->Flush(); }
	}

	void D3D11Renderer::GetViewport(float& outTopLeftX, float& outTopLeftY, float& outWidth, float& outHeight) const
	{
		outTopLeftX = mViewport->TopLeftX;
		outTopLeftY = mViewport->TopLeftY;
		outWidth	= mViewport->Width;
		outHeight	= mViewport->Height;
	}

	void D3D11Renderer::SetViewport(float topLeftX, float topLeftY, float width, float height)
	{
		ZeroMemory(mViewport, sizeof(D3D11_VIEWPORT));
		mViewport->TopLeftX = topLeftX;
		mViewport->TopLeftY = topLeftY;
		mViewport->Width	= width;
		mViewport->Height	= height;
		mViewport->MinDepth = 0.0f;
		mViewport->MaxDepth = 1.0f; // Note: important for depth buffering

		if (mContext)
			mContext->RSSetViewports(1, mViewport);
	}

	ComPtr<ID3D11Device>&		 D3D11Renderer::GetDevice() { return mDevice; }
	ComPtr<ID3D11DeviceContext>& D3D11Renderer::GetContext() { return mContext; }
	const unsigned int			 D3D11Renderer::GetNumQualityLevels() const { return mNumQualityLevels; }

	D3D11_VIEWPORT* D3D11Renderer::GetViewport()
	{
		return mViewport;
	}

	const FillMode& D3D11Renderer::GetFillMode() const
	{
		return mFillMode;
	}

	void D3D11Renderer::SetFillMode(const FillMode mode)
	{
		mFillMode = mode;
	}

	D3D11Renderer::D3D11Renderer(Core::IWindow* window)
		: mNumQualityLevels(0)
		, mViewport(DBG_NEW D3D11_VIEWPORT)
		, mFillMode(D3D11::FillMode::SOLID)
	{
		if (!Initialize(window)) throw std::runtime_error("Cannot initialize D3D11 renderer");
	}

	D3D11Renderer::~D3D11Renderer()
	{
		Reset();
		delete mViewport;
	}

	bool D3D11Renderer::Initialize(Core::IWindow* window)
	{
		D3D11::D3D11Window* win			 = static_cast<D3D11::D3D11Window*>(window);
        RECT client{};
        if (!GetClientRect(win->GetHandle(), &client)) return false;
        UINT renderWidth = static_cast<UINT>(client.right - client.left);
        UINT renderHeight = static_cast<UINT>(client.bottom - client.top);
        win->SetWidth(renderWidth);
        win->SetHeight(renderHeight);

		DX::ThrowIfFailed(
			D3D11Utils::CreateDeviceAndContext(
				win->GetHandle(), mDevice, mContext, win->GetSwapChain(), renderWidth, renderHeight, mNumQualityLevels));

		SetViewport(0, 0, static_cast<float>(renderWidth), static_cast<float>(renderHeight));
		return win->InitializeWindowRenderer(this);
	}
} // namespace D3D11