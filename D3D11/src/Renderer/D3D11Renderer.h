#pragma once
#include "Renderer/FoxtrotRenderer.h"

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
		public Core::FoxtrotRenderer
	{
	public:
		void SetViewport(
			Math::FTVector2&& topLeft,
			Math::FTVector2&& resolution);
		void SetViewport(FLOAT topLeftX, FLOAT topLeftY, FLOAT resX, FLOAT resY);
		void Reset();

	public:
		// D3D11 interfaces (Getters).
		Microsoft::WRL::ComPtr<ID3D11Device>&		 GetDevice();
		Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext();
		const unsigned int							 GetNumQualityLevels() const;
		D3D11_VIEWPORT*								 GetViewport();

	public:
		D3D11Renderer(D3D11::D3D11Window* window);
		~D3D11Renderer();

	protected:
		void RegisterMemberFuncs() override;

	protected:
		virtual bool Initialize(Core::FTWindow* window) override;

	private:
		Microsoft::WRL::ComPtr<ID3D11Device>		mDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
		D3D11_VIEWPORT*								mViewport;
		UINT										mNumQualityLevels;
	};
} // namespace D3D11