#pragma once
#include "Renderer/IRenderer.h"

#include <wrl.h>
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#define WIN32_LEAN_AND_MEAN

namespace D3D11
{
	class D3D11Window;

	class D3D11Renderer :
		public Core::IRenderer
	{
	public:
		/// @brief Returns the viewport used by this d3 d11 renderer.
		/// @param outTopLeftX Receives the viewport's horizontal origin.
		/// @param outTopLeftY Receives the viewport's vertical origin.
		/// @param outWidth Receives the viewport width.
		/// @param outHeight Receives the viewport height.
		virtual void GetViewport(float& outTopLeftX, float& outTopLeftY, float& outWidth, float& outHeight) const override;
		/// @brief Updates the viewport used by subsequent operations.
		/// @param topLeftX Horizontal origin of the viewport or rectangle.
		/// @param topLeftY Vertical origin of the viewport or rectangle.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		virtual void SetViewport(float topLeftX, float topLeftY, float width, float height) override;

	public:
		// D3D11 interfaces (Getters).
		/// @brief Returns the device used by this d3 d11 renderer.
		/// @return Borrowed access to the device.
		/// @note Changes through the returned reference affect this object's stored state.
		virtual Microsoft::WRL::ComPtr<ID3D11Device>&		 GetDevice();
		/// @brief Returns the context used by this d3 d11 renderer.
		/// @return Borrowed access to the context.
		/// @note Changes through the returned reference affect this object's stored state.
		virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext();
		/// @brief Returns the viewport used by this d3 d11 renderer.
		/// @return Borrowed access to the viewport.
		virtual D3D11_VIEWPORT*								 GetViewport();
		/// @brief Returns the num quality levels used by this d3 d11 renderer.
		/// @return Current num quality levels.
		const unsigned int									 GetNumQualityLevels() const;
		/// @brief Returns the fill mode used by this d3 d11 renderer.
		/// @return Borrowed access to the fill mode.
		const Core::FillMode&								 GetFillMode() const override;

		/// @brief Updates the fill mode used by subsequent operations.
		/// @param mode Replacement fill mode.
		void SetFillMode(const Core::FillMode mode);

	public:
		/// @brief Initializes the Direct3D device, context, and rendering resources.
		/// @param window Window used by the operation.
		/// @throws std::runtime_error If the operation encounters the failure condition checked by this implementation.
		D3D11Renderer(Core::IWindow* window);
		/// @brief Releases the resources managed by this instance during destruction.
		~D3D11Renderer();

	protected:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param window Window used by the operation.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		virtual bool Initialize(Core::IWindow* window);
		/// @brief Clears transient state before the next processing cycle.
		void		 Reset();

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>		mDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
		D3D11_VIEWPORT*								mViewport;
		UINT										mNumQualityLevels;
		Graphics::FillMode							mFillMode;
	};
} // namespace D3D11
