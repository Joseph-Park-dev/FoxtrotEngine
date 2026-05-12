// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "D3D11Utils.h"

#include <DDSTextureLoader.h> // For reading Cubemaps
#include <dxgi.h>			  // DXGIFactory
#include <dxgi1_4.h>		  // DXGIFactory4
#include <stdio.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

#include "FTDS/Dynamic/DynamicArray.h"
#include "Debugging/DebugFuncs.h"
#include "DebugFuncs.h"

namespace D3D11
{
	using namespace Common;
	using namespace Core;
	using namespace Math;
	using namespace std;
	using Microsoft::WRL::ComPtr;

	HRESULT D3D11Utils::CreateDeviceAndContext(
		const HWND					 window,
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context,
		ComPtr<IDXGISwapChain>&		 swapChain,
		const UINT&					 renderWidth,
		const UINT&					 renderHeight,
		UINT&						 numQualityLevel)
	{
		const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

		int createDeviceFlags = 0;
#if defined(FOXTROT_EDITOR) || defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		Microsoft::WRL::ComPtr<ID3D11Device>		deviceCache;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> contextCache;

		const D3D_FEATURE_LEVEL featureLevels[2] = {
			D3D_FEATURE_LEVEL_11_0, // The highest DX version.
			D3D_FEATURE_LEVEL_9_3
		};
		D3D_FEATURE_LEVEL featureLevel;

		HRESULT hr = S_OK;

		hr = D3D11CreateDevice(
			nullptr,					// Specify nullptr to use the default adapter.
			driverType,					// Create a device using the hardware graphics driver.
			0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
			createDeviceFlags,			// Set debug and Direct2D compatibility flags.
			featureLevels,				// List of feature levels this app can support.
			ARRAYSIZE(featureLevels),	// Size of the list above.
			D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
			deviceCache.GetAddressOf(), // Returns the Direct3D device created.
			&featureLevel,				// Returns feature level of device created.
			contextCache.GetAddressOf() // Returns the device immediate context.
		);
		if (FAILED(hr))
		{
			printf("D3D11CreateDevice() failed.");
			return hr;
		}

		if (featureLevel != D3D_FEATURE_LEVEL_11_0)
		{
			printf("D3D Feature Level 11 is unsupported.");
			return E_FAIL;
		}

		// ����: Immediate vs deferred context
		// A deferred context is primarily used for multi-threading and is not necessary for a
		// single-threaded application.
		// https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-devices-intro#deferred-context

		// 4X MSAA compatibility check
		deviceCache->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevel);
		if (numQualityLevel <= 0)
		{
			printf("MSAA not supported.");
		}

		numQualityLevel = 0; // Disable MSAA;

		hr = deviceCache.As(&device);
		if (FAILED(hr))
		{
			printf("device.AS() failed.");
			return hr;
		}

		hr = contextCache.As(&context);
		if (FAILED(hr))
		{
			printf("context.As() failed.");
			return hr;
		}

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferDesc.Width					  = renderWidth;				// set the back buffer width
		sd.BufferDesc.Height				  = renderHeight;				// set the back buffer height
		sd.BufferDesc.Format				  = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
		sd.BufferCount						  = 2;							// Double-buffering
		sd.BufferDesc.RefreshRate.Numerator	  = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;

		sd.BufferUsage	= DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT; // how swap chain is to be used
		sd.OutputWindow = window;													 // the window to be used
		sd.Windowed		= TRUE;														 // windowed/full-screen mode
		sd.Flags		= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;					 // allow full-screen switching
		sd.SwapEffect	= DXGI_SWAP_EFFECT_DISCARD;
		if (numQualityLevel > 0)
		{
			sd.SampleDesc.Count	  = 4; // how many multi-samples
			sd.SampleDesc.Quality = numQualityLevel - 1;
		}
		else
		{
			sd.SampleDesc.Count	  = 1; // how many multi-samples
			sd.SampleDesc.Quality = 0;
		}

		hr = D3D11CreateDeviceAndSwapChain(
			0, // Default adapter
			driverType,
			0, // No software device
			createDeviceFlags,
			featureLevels,
			1,
			D3D11_SDK_VERSION,
			&sd,
			swapChain.GetAddressOf(),
			device.GetAddressOf(),
			&featureLevel,
			context.GetAddressOf());
		if (FAILED(hr))
		{
			printf("D3D11CreateDeviceAndSwapChain() failed.");
			return hr;
		}
		return hr;
	}

	HRESULT D3D11Utils::CreateSwapChain(const HWND window, ComPtr<ID3D11Device>& device, ComPtr<IDXGISwapChain>& swapChain, int renderW, int renderH, UINT numQualityLevel)
	{
		// Get the DXGI factory
		IDXGIDevice* pDXGIDevice = nullptr;
		HRESULT		 hr			 = device->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
		if (FAILED(hr))
		{
			Debug::LogError(__LINE__, __FILE__, "Failed to retrieve DXGI device");
			return hr;
		}

		IDXGIAdapter* pDXGIAdapter = nullptr;
		hr						   = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);
		pDXGIDevice->Release();
		if (FAILED(hr))
		{
			Debug::LogError(__LINE__, __FILE__, "Failed to retrieve DXGI adapter");
			return hr;
		}

		IDXGIFactory* pDXGIFactory = nullptr;
		hr						   = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);
		pDXGIAdapter->Release();
		if (FAILED(hr))
		{
			Debug::LogError(__LINE__, __FILE__, "Failed to retrieve DXGI factory");
			return hr;
		}

		// Describe the swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferDesc.Width					  = renderW;					// set the back buffer width
		sd.BufferDesc.Height				  = renderH;					// set the back buffer height
		sd.BufferDesc.Format				  = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
		sd.BufferCount						  = 2;							// Double-buffering
		sd.BufferDesc.RefreshRate.Numerator	  = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;

		sd.BufferUsage	= DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT; // how swap chain is to be used
		sd.OutputWindow = window;													 // the window to be used
		sd.Windowed		= TRUE;														 // windowed/full-screen mode
		sd.Flags		= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;					 // allow full-screen switching
		sd.SwapEffect	= DXGI_SWAP_EFFECT_DISCARD;

		if (numQualityLevel > 0)
		{
			sd.SampleDesc.Count	  = 4; // how many multi-samples
			sd.SampleDesc.Quality = numQualityLevel - 1;
		}
		else
		{
			sd.SampleDesc.Count	  = 1; // how many multi-samples
			sd.SampleDesc.Quality = 0;
		}

		// Create the swap chain
		hr = pDXGIFactory->CreateSwapChain(device.Get(), &sd, swapChain.GetAddressOf());
		pDXGIFactory->Release();
		if (FAILED(hr))
		{
			Debug::LogError(__LINE__, __FILE__, "Failed to create swap chain");
			return hr;
		}
		return S_OK;
	}

	HRESULT D3D11Utils::CreateDepthBuffer(
		ComPtr<ID3D11Device>&			device,
		int								renderWidth,
		int								renderHeight,
		UINT							numQualityLevels,
		ComPtr<ID3D11DepthStencilView>& depthStencilView)
	{
		depthStencilView.Reset();

		D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
		depthStencilBufferDesc.Width	 = renderWidth;
		depthStencilBufferDesc.Height	 = renderHeight;
		depthStencilBufferDesc.MipLevels = 1;
		depthStencilBufferDesc.ArraySize = 1;
		depthStencilBufferDesc.Format	 = DXGI_FORMAT_D24_UNORM_S8_UINT;
		if (numQualityLevels > 0)
		{
			depthStencilBufferDesc.SampleDesc.Count	  = 4; // how many multisamples
			depthStencilBufferDesc.SampleDesc.Quality = numQualityLevels - 1;
		}
		else
		{
			depthStencilBufferDesc.SampleDesc.Count	  = 1; // how many multisamples
			depthStencilBufferDesc.SampleDesc.Quality = 0;
		}
		depthStencilBufferDesc.Usage		  = D3D11_USAGE_DEFAULT;
		depthStencilBufferDesc.BindFlags	  = D3D11_BIND_DEPTH_STENCIL;
		depthStencilBufferDesc.CPUAccessFlags = 0;
		depthStencilBufferDesc.MiscFlags	  = 0;

		ComPtr<ID3D11Texture2D> depthStencilBuffer;

		if (FAILED(device->CreateTexture2D(&depthStencilBufferDesc, 0, depthStencilBuffer.GetAddressOf())))
		{
			std::cout << "CreateTexture2D() failed." << std::endl;
			return E_FAIL;
		}
		if (FAILED(device->CreateDepthStencilView(
				depthStencilBuffer.Get(), 0, depthStencilView.GetAddressOf())))
		{
			std::cout << "CreateDepthStencilView() failed." << std::endl;
			return E_FAIL;
		}

		return S_OK;
	}

	HRESULT D3D11Utils::CreateVertexShaderAndInputLayout(
		ComPtr<ID3D11Device>&			device,
		const wstring&					filename,
		const D3D11_INPUT_ELEMENT_DESC* inputElements,
		size_t							inputElementsSize,
		ComPtr<ID3D11VertexShader>&		vertexShader,
		ComPtr<ID3D11InputLayout>&		inputLayout)
	{

		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// ���̴��� �������� �̸��� "main"�� �Լ��� ����
		// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
		HRESULT hr = D3DCompileFromFile(
			filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", compileFlags, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());

		CheckResult(hr, errorBlob.Get());

		DX::ThrowIfFailed(
			device->CreateVertexShader(
				shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, vertexShader.GetAddressOf()));

		DX::ThrowIfFailed(
			device->CreateInputLayout(
				inputElements,
				UINT(inputElementsSize),
				shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(),
				inputLayout.GetAddressOf()));
		return hr;
	}

	HRESULT D3D11Utils::CreateVertexShaderAndInputLayout(
		ComPtr<ID3D11Device>&					device,
		const wstring&							filename,
		const vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
		ComPtr<ID3D11VertexShader>&				vertexShader,
		ComPtr<ID3D11InputLayout>&				inputLayout)
	{

		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// ���̴��� �������� �̸��� "main"�� �Լ��� ����
		// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
		HRESULT hr = D3DCompileFromFile(
			filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", compileFlags, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());

		CheckResult(hr, errorBlob.Get());

		DX::ThrowIfFailed(
			device->CreateVertexShader(
				shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, vertexShader.GetAddressOf()));

		DX::ThrowIfFailed(
			device->CreateInputLayout(
				inputElements.data(),
				UINT(inputElements.size()),
				shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(),
				inputLayout.GetAddressOf()));
		return hr;
	}

	HRESULT D3D11Utils::CreateVertexShaderAndInputLayout(
		ComPtr<ID3D11Device>&								device,
		const wstring&										filename,
		const Common::FTDS::DynamicArray<D3D11_INPUT_ELEMENT_DESC>& inputElements,
		ComPtr<ID3D11VertexShader>&							vertexShader,
		ComPtr<ID3D11InputLayout>&							inputLayout)
	{

		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// ���̴��� �������� �̸��� "main"�� �Լ��� ����
		// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
		HRESULT hr = D3DCompileFromFile(
			filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", compileFlags, 0, shaderBlob.GetAddressOf(), errorBlob.GetAddressOf());

		CheckResult(hr, errorBlob.Get());

		DX::ThrowIfFailed(
			device->CreateVertexShader(
				shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, vertexShader.GetAddressOf()));

		DX::ThrowIfFailed(
			device->CreateInputLayout(
				inputElements.Data(),
				UINT(inputElements.GetSize()),
				shaderBlob->GetBufferPointer(),
				shaderBlob->GetBufferSize(),
				inputLayout.GetAddressOf()));
		return hr;
	}

	HRESULT D3D11Utils::CreateRenderTargetView(
		ComPtr<ID3D11RenderTargetView>& rtv,
		ComPtr<ID3D11Device>&			device,
		ComPtr<IDXGISwapChain>&			swapChain)
	{
		rtv.Reset();
		ComPtr<ID3D11Texture2D> backBuffer;
		swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		if (backBuffer)
		{
			HRESULT resultRTV;
			resultRTV = device->CreateRenderTargetView(
				backBuffer.Get(), nullptr, rtv.GetAddressOf());

			backBuffer.Reset();
			return resultRTV;
		}
		else
		{
			printf("ERROR : FoxtrotRenderer::CreateRenderTargetView() -> BackBuffer not set");
			return E_FAIL;
		}
	}

	HRESULT D3D11Utils::CreateRenderTargetView(
		ComPtr<ID3D11RenderTargetView>& RTV,
		ComPtr<ID3D11Device>&			device,
		ComPtr<IDXGISwapChain>&			swapChain,
		ComPtr<ID3D11Texture2D>&		indexTexture,
		ComPtr<ID3D11Texture2D>&		indexTempTexture,
		ComPtr<ID3D11Texture2D>&		indexStagingTexture)
	{
		RTV.Reset();
		ComPtr<ID3D11Texture2D> backBuffer;
		swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
		if (backBuffer)
		{
			HRESULT resultRTV;
			// resultRTV = device->CreateRenderTargetView(
			//	backBuffer.Get(), nullptr, RTV.GetAddressOf());

			D3D11_TEXTURE2D_DESC desc;
			backBuffer->GetDesc(&desc);
			desc.SampleDesc.Count	= 1;
			desc.SampleDesc.Quality = 0;
			desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags			= 0;

			device->CreateTexture2D(
				&desc, nullptr, indexTempTexture.GetAddressOf());

			// Creating 1x1 sized staging texture
			desc.BindFlags		= 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.Usage			= D3D11_USAGE_STAGING;
			desc.Width			= 1;
			desc.Height			= 1;

			resultRTV = device->CreateTexture2D(
				&desc, nullptr, indexStagingTexture.GetAddressOf());

			backBuffer->GetDesc(&desc); // Same desc with "backBuffer"
			resultRTV = device->CreateTexture2D(&desc, nullptr, indexTexture.GetAddressOf());

			resultRTV = device->CreateRenderTargetView(
				indexTexture.Get(), nullptr, RTV.GetAddressOf());

			return resultRTV;
		}
		else
		{
			printf("ERROR : FoxtrotRenderer::CreateRenderTargetView() -> BackBuffer not set");
			return E_FAIL;
		}
	}

	void D3D11Utils::CreateHullShader(ComPtr<ID3D11Device>& device, const wstring& filename, ComPtr<ID3D11HullShader>& hullShader)
	{
		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// ���̴��� �������� �̸��� "main"�� �Լ��� ����
		// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
		HRESULT hr = D3DCompileFromFile(
			filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "hs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

		CheckResult(hr, errorBlob.Get());

		device->CreateHullShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &hullShader);
	}

	void D3D11Utils::CreateDomainShader(
		ComPtr<ID3D11Device>& device, const wstring& filename, ComPtr<ID3D11DomainShader>& domainShader)
	{
		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// ���̴��� �������� �̸��� "main"�� �Լ��� ����
		// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
		HRESULT hr = D3DCompileFromFile(
			filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ds_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

		CheckResult(hr, errorBlob.Get());

		device->CreateDomainShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &domainShader);
	}

	HRESULT D3D11Utils::CreatePixelShader(ComPtr<ID3D11Device>& device, const wstring& filename, ComPtr<ID3D11PixelShader>& pixelShader)
	{
		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// ���̴��� �������� �̸��� "main"�� �Լ��� ����
		// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
		HRESULT hr = D3DCompileFromFile(
			filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

		CheckResult(hr, errorBlob.Get());

		hr = device->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &pixelShader);
		CheckResult(hr, errorBlob.Get());
		return hr;
	}

	void D3D11Utils::CreateIndexBuffer(ComPtr<ID3D11Device>& device, const std::vector<uint32_t>& indices, ComPtr<ID3D11Buffer>& indexBuffer)
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage			   = D3D11_USAGE_DYNAMIC; // �ʱ�ȭ �� ����X
		bufferDesc.ByteWidth		   = UINT(sizeof(uint32_t) * indices.size());
		bufferDesc.BindFlags		   = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags	   = D3D11_CPU_ACCESS_WRITE; // 0 if no CPU access is necessary.
		bufferDesc.StructureByteStride = sizeof(uint32_t);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem				   = indices.data();
		indexBufferData.SysMemPitch			   = 0;
		indexBufferData.SysMemSlicePitch	   = 0;

		DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, &indexBufferData, indexBuffer.GetAddressOf()));
	}

	void D3D11Utils::CreateIndexBuffer(ComPtr<ID3D11Device>& device, Common::FTDS::DynamicArray<uint32_t>& indices, ComPtr<ID3D11Buffer>& indexBuffer)
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage			   = D3D11_USAGE_DYNAMIC; // �ʱ�ȭ �� ����X
		bufferDesc.ByteWidth		   = UINT(sizeof(uint32_t) * indices.GetSize());
		bufferDesc.BindFlags		   = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags	   = D3D11_CPU_ACCESS_WRITE; // 0 if no CPU access is necessary.
		bufferDesc.StructureByteStride = sizeof(uint32_t);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem				   = indices.Data();
		indexBufferData.SysMemPitch			   = 0;
		indexBufferData.SysMemSlicePitch	   = 0;

		DX::ThrowIfFailed(device->CreateBuffer(&bufferDesc, &indexBufferData, indexBuffer.GetAddressOf()));
	}

	void D3D11Utils::CreateIndexBuffer(ComPtr<ID3D11Device>& device, UINT numIndices, ComPtr<ID3D11Buffer>& indexBuffer)
	{
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
		indexBufferDesc.Usage		   = D3D11_USAGE_DYNAMIC;
		indexBufferDesc.ByteWidth	   = sizeof(uint32_t) * numIndices;
		indexBufferDesc.BindFlags	   = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		DX::ThrowIfFailed(device->CreateBuffer(&indexBufferDesc, nullptr, &indexBuffer));
	}

	void D3D11Utils::CreateGeometryShader(
		ComPtr<ID3D11Device>& device, const wstring& filename, ComPtr<ID3D11GeometryShader>& geometryShader)
	{

		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;

		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		// ���̴��� �������� �̸��� "main"�� �Լ��� ����
		// D3D_COMPILE_STANDARD_FILE_INCLUDE �߰�: ���̴����� include ���
		HRESULT hr = D3DCompileFromFile(
			filename.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0", compileFlags, 0, &shaderBlob, &errorBlob);

		// CheckResult(hr, errorBlob.Get());

		device->CreateGeometryShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &geometryShader);
	}

	void D3D11Utils::ReadImage(Common::FTDS::String filename, std::vector<uint8_t>& image, int& width, int& height)
	{
		int channels;

		unsigned char* img =
			stbi_load(filename.C_Str(), &width, &height, &channels, 0);

		// 4ä�η� ���� ����
		image.resize(width * height * 4);

		if (channels == 1)
		{
			for (size_t i = 0; i < width * height; i++)
			{
				uint8_t g = img[i * channels + 0];
				for (size_t c = 0; c < 4; c++)
				{
					image[4 * i + c] = g;
				}
			}
		}
		else if (channels == 3)
		{
			for (size_t i = 0; i < width * height; i++)
			{
				for (size_t c = 0; c < 3; c++)
				{
					image[4 * i + c] = img[i * channels + c];
				}
				image[4 * i + 3] = 255;
			}
		}
		else if (channels == 4)
		{
			for (size_t i = 0; i < width * height; i++)
			{
				for (size_t c = 0; c < 4; c++)
				{
					image[4 * i + c] = img[i * channels + c];
				}
			}
		}
		else
		{
			std::cout << "Cannot read " << channels << " channels" << endl;
		}
		delete[] img;
	}

	ComPtr<ID3D11Texture2D>
	D3D11Utils::CreateStagingTexture(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context,
		const int					 width,
		const int					 height,
		const std::vector<uint8_t>&	 image,
		const int					 mipLevels,
		const int					 arraySize)
	{
		// Create a staging texture.
		D3D11_TEXTURE2D_DESC txtDesc;
		ZeroMemory(&txtDesc, sizeof(txtDesc));
		txtDesc.Width			 = width;
		txtDesc.Height			 = height;
		txtDesc.MipLevels		 = mipLevels;
		txtDesc.ArraySize		 = arraySize;
		txtDesc.Format			 = DXGI_FORMAT_R8G8B8A8_UNORM;
		txtDesc.SampleDesc.Count = 1;
		txtDesc.Usage			 = D3D11_USAGE_STAGING;
		txtDesc.CPUAccessFlags	 = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;

		ComPtr<ID3D11Texture2D> stagingTexture;
		if (FAILED(device->CreateTexture2D(&txtDesc, nullptr, stagingTexture.GetAddressOf())))
		{
			cout << "Failed()" << endl;
		}

		// Copy 2D image from CPU.
		D3D11_MAPPED_SUBRESOURCE ms;
		context->Map(stagingTexture.Get(), NULL, D3D11_MAP_WRITE, NULL, &ms);
		uint8_t* pData = (uint8_t*)ms.pData;

		// Copy image data line by line.
		for (UINT h = 0; h < UINT(height); h++)
			memcpy(&pData[h * ms.RowPitch], &image[h * width * 4], width * sizeof(uint8_t) * 4);

		context->Unmap(stagingTexture.Get(), NULL);

		return stagingTexture;
	}

	void D3D11Utils::CreateTextureArray(
		ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, const std::vector<Common::FTDS::String> filenames, ComPtr<ID3D11Texture2D>& texture, ComPtr<ID3D11ShaderResourceView>& textureResourceView)
	{

		using namespace std;

		if (filenames.empty())
			return;

		// ��� �̹����� width�� height�� ���ٰ� �����մϴ�.

		// ���Ϸκ��� �̹��� ���� ���� �о���Դϴ�.
		int						width = 0, height = 0;
		vector<vector<uint8_t>> imageArray;
		for (const auto& f : filenames)
		{
			std::vector<uint8_t> image;

			ReadImage(f, image, width, height);
			imageArray.push_back(image);
		}

		UINT size = UINT(filenames.size());

		// Texture2DArray�� ����ϴ�. �̶� �����͸� CPU�κ��� �������� �ʽ��ϴ�.
		D3D11_TEXTURE2D_DESC txtDesc;
		ZeroMemory(&txtDesc, sizeof(txtDesc));
		txtDesc.Width			   = UINT(width);
		txtDesc.Height			   = UINT(height);
		txtDesc.MipLevels		   = 0; // �Ӹ� ���� �ִ�
		txtDesc.ArraySize		   = size;
		txtDesc.Format			   = DXGI_FORMAT_R8G8B8A8_UNORM;
		txtDesc.SampleDesc.Count   = 1;
		txtDesc.SampleDesc.Quality = 0;
		txtDesc.Usage			   = D3D11_USAGE_DEFAULT; // ������¡ �ؽ���κ��� ���� ����
		txtDesc.BindFlags		   = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		txtDesc.MiscFlags		   = D3D11_RESOURCE_MISC_GENERATE_MIPS; // �Ӹ� ���

		// �ʱ� ������ ���� �ؽ��縦 ����ϴ�.
		device->CreateTexture2D(&txtDesc, nullptr, texture.GetAddressOf());

		// ������ ������� MipLevels�� Ȯ��
		texture->GetDesc(&txtDesc);
		// cout << txtDesc.MipLevels << endl;

		// StagingTexture�� ���� �ϳ��� �����մϴ�.
		for (size_t i = 0; i < imageArray.size(); i++)
		{

			auto& image = imageArray[i];

			// StagingTexture�� Texture2DArray�� �ƴ϶� Texture2D �Դϴ�.
			ComPtr<ID3D11Texture2D> stagingTexture =
				CreateStagingTexture(device, context, width, height, image, 1, 1);

			// ������¡ �ؽ��縦 �ؽ��� �迭�� �ش� ��ġ�� �����մϴ�.
			UINT subresourceIndex =
				D3D11CalcSubresource(0, UINT(i), txtDesc.MipLevels);

			context->CopySubresourceRegion(texture.Get(), subresourceIndex, 0, 0, 0, stagingTexture.Get(), 0, nullptr);
		}

		device->CreateShaderResourceView(texture.Get(), nullptr, textureResourceView.GetAddressOf());

		context->GenerateMips(textureResourceView.Get());
	}

	// HRESULT D3D11Utils::CreateCubemapTexture(
	//	ComPtr<ID3D11Device>& device, FTTexture* texture)
	//{
	//	ComPtr<ID3D11Texture2D> textureBuf;
	//
	//	wchar_t* path = texture->RelativePath().WC_Str();
	//	HRESULT result = CreateDDSTextureFromFileEx(
	//		device.Get(), path, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, DDS_LOADER_FLAGS(false), (ID3D11Resource**)textureBuf.GetAddressOf(), texture->GetResourceView().GetAddressOf(), nullptr);
	//	delete[] path;
	//	return result;
	// }

	void D3D11Utils::WriteToFile(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11Texture2D>& textureToWrite, Common::FTDS::String& filename)
	{
		D3D11_TEXTURE2D_DESC desc;
		textureToWrite->GetDesc(&desc);
		desc.SampleDesc.Count	= 1;
		desc.SampleDesc.Quality = 0;
		desc.BindFlags			= 0;
		desc.MiscFlags			= 0;
		desc.CPUAccessFlags		= D3D11_CPU_ACCESS_READ; // CPU���� �б� ����
		desc.Usage				= D3D11_USAGE_STAGING;	 // GPU���� CPU�� ���� �����͸� �ӽ� ����

		ComPtr<ID3D11Texture2D> stagingTexture;
		if (FAILED(device->CreateTexture2D(&desc, nullptr, stagingTexture.GetAddressOf())))
		{
			cout << "Failed()" << endl;
		}

		// ����: ��ü ������ ��
		// context->CopyResource(stagingTexture.Get(), pTemp.Get());

		// �Ϻθ� ������ �� ���
		D3D11_BOX box;
		box.left   = 0;
		box.right  = desc.Width;
		box.top	   = 0;
		box.bottom = desc.Height;
		box.front  = 0;
		box.back   = 1;
		context->CopySubresourceRegion(stagingTexture.Get(), 0, 0, 0, 0, textureToWrite.Get(), 0, &box);

		// R8G8B8A8 �̶�� ����
		std::vector<uint8_t> pixels(desc.Width * desc.Height * 4);

		D3D11_MAPPED_SUBRESOURCE ms;
		context->Map(stagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
					 &ms); // D3D11_MAP_READ ����

		// �ؽ��簡 ���� ��쿡��
		// ms.RowPitch�� width * sizeof(uint8_t) * 4���� Ŭ ���� �־
		// for������ ������ �ϳ��� ����
		uint8_t* pData = (uint8_t*)ms.pData;
		for (unsigned int h = 0; h < desc.Height; h++)
		{
			memcpy(&pixels[h * desc.Width * 4], &pData[h * ms.RowPitch], desc.Width * sizeof(uint8_t) * 4);
		}

		context->Unmap(stagingTexture.Get(), NULL);

		stbi_write_png(filename.C_Str(), desc.Width, desc.Height, 4, pixels.data(), desc.Width * 4);
	}
} // namespace D3D11