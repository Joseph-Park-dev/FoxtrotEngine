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

	/// @brief Clears transient state before the next processing cycle.
	void D3D11Renderer::Reset()
	{
		if (mContext) { mContext->ClearState(); mContext->Flush(); }
	}

	/// @brief Returns the viewport used by this d3 d11 renderer.
	/// @param outTopLeftX Receives the viewport's horizontal origin.
	/// @param outTopLeftY Receives the viewport's vertical origin.
	/// @param outWidth Receives the viewport width.
	/// @param outHeight Receives the viewport height.
	void D3D11Renderer::GetViewport(float& outTopLeftX, float& outTopLeftY, float& outWidth, float& outHeight) const
	{
		outTopLeftX = mViewport->TopLeftX;
		outTopLeftY = mViewport->TopLeftY;
		outWidth	= mViewport->Width;
		outHeight	= mViewport->Height;
	}

	/// @brief Updates the viewport used by subsequent operations.
	/// @param topLeftX Horizontal origin of the viewport or rectangle.
	/// @param topLeftY Vertical origin of the viewport or rectangle.
	/// @param width Width of the window, texture, or geometry.
	/// @param height Height of the window, texture, or geometry.
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

	/// @brief Returns the device used by this d3 d11 renderer.
	/// @return Borrowed access to the device.
	/// @note Changes through the returned reference affect this object's stored state.
	ComPtr<ID3D11Device>&		 D3D11Renderer::GetDevice() { return mDevice; }
	/// @brief Returns the context used by this d3 d11 renderer.
	/// @return Borrowed access to the context.
	/// @note Changes through the returned reference affect this object's stored state.
	ComPtr<ID3D11DeviceContext>& D3D11Renderer::GetContext() { return mContext; }
	/// @brief Returns the num quality levels used by this d3 d11 renderer.
	/// @return Current num quality levels.
	const unsigned int			 D3D11Renderer::GetNumQualityLevels() const { return mNumQualityLevels; }

	/// @brief Returns the viewport used by this d3 d11 renderer.
	/// @return Borrowed access to the viewport.
	D3D11_VIEWPORT* D3D11Renderer::GetViewport()
	{
		return mViewport;
	}

	/// @brief Returns the fill mode used by this d3 d11 renderer.
	/// @return Borrowed access to the fill mode.
	const FillMode& D3D11Renderer::GetFillMode() const
	{
		return mFillMode;
	}

	/// @brief Updates the fill mode used by subsequent operations.
	/// @param mode Replacement fill mode.
	void D3D11Renderer::SetFillMode(const FillMode mode)
	{
		mFillMode = mode;
	}

	/// @brief Initializes the Direct3D device, context, and rendering resources.
	/// @param window Window used by the operation.
	/// @note Initializes the :D3D11Renderer base or delegates to its constructor.
	/// @throws std::runtime_error If the operation encounters the failure condition checked by this implementation.
	D3D11Renderer::D3D11Renderer(Core::IWindow* window)
		: mNumQualityLevels(0)
		, mViewport(DBG_NEW D3D11_VIEWPORT)
		, mFillMode(D3D11::FillMode::SOLID)
	{
		if (!Initialize(window)) throw std::runtime_error("Cannot initialize D3D11 renderer");
	}

	/// @brief Releases the resources managed by this instance during destruction.
	D3D11Renderer::~D3D11Renderer()
	{
		Reset();
		delete mViewport;
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param window Window used by the operation.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
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
