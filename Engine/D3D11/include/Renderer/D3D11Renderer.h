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
		virtual void GetViewport(float& outTopLeftX, float& outTopLeftY, float& outWidth, float& outHeight) const override;
		virtual void SetViewport(float topLeftX, float topLeftY, float width, float height) override;

	public:
		// D3D11 interfaces (Getters).
		virtual Microsoft::WRL::ComPtr<ID3D11Device>&		 GetDevice() override;
		virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext() override;
		virtual D3D11_VIEWPORT*								 GetViewport() override;
		const unsigned int									 GetNumQualityLevels() const override;
		const Core::FillMode&								 GetFillMode() const;

		void SetFillMode(const Core::FillMode mode);

	public:
		D3D11Renderer(Core::IWindow* window);
		~D3D11Renderer();

	protected:
		virtual bool Initialize(Core::IWindow* window) override;
		void		 Reset();

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>		mDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
		D3D11_VIEWPORT*								mViewport;
		UINT										mNumQualityLevels;
		Core::FillMode								mFillMode;
	};
} // namespace D3D11