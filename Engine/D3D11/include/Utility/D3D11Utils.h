// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Utility class for DirectX 11 functionalities.
/// </summary>

#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <windows.h>
#include <wrl/client.h> // Microsoft::WRL::ComPtr

#include "ResourceSystem/FTTexture.h"
#include "TemplateFunctions.h"
#include "FTDS/Static/FTString.h"
#include "DebugFuncs.h"

namespace D3D11
{
	using namespace Common;
	class Camera;
	inline HRESULT CheckResult(HRESULT hr, ID3DBlob* errorBlob)
	{
		if (FAILED(hr))
		{
			// 파일이 없을 경우
			if ((hr & D3D11_ERROR_FILE_NOT_FOUND) != 0)
			{
				std::cout << "File not found." << std::endl;
			}

			// 에러 메시지가 있으면 출력
			if (errorBlob)
			{
				std::cout << "Shader compile error\n"
						  << (char*)errorBlob->GetBufferPointer() << std::endl;
			}
		}
		return hr;
	}

	class D3D11Utils
	{
	public:
		static HRESULT CreateDeviceAndContext(
			const HWND									 window,
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
			Microsoft::WRL::ComPtr<IDXGISwapChain>&		 swapChain,
			const UINT&									 renderWidth,
			const UINT&									 renderHeight,
			UINT&										 numQualityLevel);

		static HRESULT CreateSwapChain(
			const HWND								window,
			Microsoft::WRL::ComPtr<ID3D11Device>&	device,
			Microsoft::WRL::ComPtr<IDXGISwapChain>& swapChain,
			int										renderWidth,
			int										renderHeight,
			UINT									numQualityLevel);

		static HRESULT
		CreateDepthBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>&			device,
			int												renderWidth,
			int												renderHeight,
			UINT											numQualityLevels,
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView);

		static HRESULT CreateVertexShaderAndInputLayout(
			Microsoft::WRL::ComPtr<ID3D11Device>&		device,
			const std::wstring&							filename,
			const D3D11_INPUT_ELEMENT_DESC*				inputElements,
			size_t										inputElementsSize,
			Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader,
			Microsoft::WRL::ComPtr<ID3D11InputLayout>&	inputLayout);

		static HRESULT CreateVertexShaderAndInputLayout(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			const std::wstring&							 filename,
			const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
			Microsoft::WRL::ComPtr<ID3D11VertexShader>&	 vertexShader,
			Microsoft::WRL::ComPtr<ID3D11InputLayout>&	 inputLayout);

		static HRESULT CreateVertexShaderAndInputLayout(
			Microsoft::WRL::ComPtr<ID3D11Device>&					  device,
			const std::wstring&										  filename,
			const Common::FTDS::DynamicArray<D3D11_INPUT_ELEMENT_DESC>& inputElements,
			Microsoft::WRL::ComPtr<ID3D11VertexShader>&				  vertexShader,
			Microsoft::WRL::ComPtr<ID3D11InputLayout>&				  inputLayout);

		static HRESULT CreateRenderTargetView(
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& RTV,
			Microsoft::WRL::ComPtr<ID3D11Device>&			device,
			Microsoft::WRL::ComPtr<IDXGISwapChain>&			swapChain);

		static HRESULT CreateRenderTargetView(
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& RTV,
			Microsoft::WRL::ComPtr<ID3D11Device>&			device,
			Microsoft::WRL::ComPtr<IDXGISwapChain>&			swapChain,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>&		indexTexture,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>&		indexTempTexture,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>&		indexStagingTexture);

		static void CreateHullShader(
			Microsoft::WRL::ComPtr<ID3D11Device>&	  device,
			const std::wstring&						  filename,
			Microsoft::WRL::ComPtr<ID3D11HullShader>& hullShader);

		static void CreateDomainShader(
			Microsoft::WRL::ComPtr<ID3D11Device>&		device,
			const std::wstring&							filename,
			Microsoft::WRL::ComPtr<ID3D11DomainShader>& domainShader);

		static void
		CreateGeometryShader(
			Microsoft::WRL::ComPtr<ID3D11Device>&		  device,
			const std::wstring&							  filename,
			Microsoft::WRL::ComPtr<ID3D11GeometryShader>& geometryShader);

		static HRESULT CreatePixelShader(
			Microsoft::WRL::ComPtr<ID3D11Device>&	   device,
			const std::wstring&						   filename,
			Microsoft::WRL::ComPtr<ID3D11PixelShader>& pixelShader);

		static void CreateIndexBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const std::vector<uint32_t>&		  indices,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer);

		static void CreateIndexBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			Common::FTDS::DynamicArray<uint32_t>&	  indices,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer);

		static void CreateIndexBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			UINT								  numIndices,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer);

		template <typename T_VERTEX>
		static void CreateVertexBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const std::vector<T_VERTEX>&		  vertices,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer)
		{
			// D3D11_USAGE enumeration (d3d11.h)
			// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage			   = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth		   = UINT(sizeof(T_VERTEX) * vertices.size());
			bufferDesc.BindFlags		   = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags	   = D3D11_CPU_ACCESS_WRITE; // 0 if no CPU access is necessary.
			bufferDesc.StructureByteStride = sizeof(T_VERTEX);

			D3D11_SUBRESOURCE_DATA vertexBufferData = {
				0
			}; // MS 예제에서 초기화하는 방식
			vertexBufferData.pSysMem		  = vertices.data();
			vertexBufferData.SysMemPitch	  = 0;
			vertexBufferData.SysMemSlicePitch = 0;

			const HRESULT hr = device->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
			DX::ThrowIfFailed(hr);
		}

		template <typename T_VERTEX>
		static void CreateVertexBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			T_VERTEX*							  vertices,
			size_t								  vertexCount,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer)
		{
			// D3D11_USAGE enumeration (d3d11.h)
			// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage			   = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth		   = UINT(sizeof(T_VERTEX) * vertexCount);
			bufferDesc.BindFlags		   = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags	   = D3D11_CPU_ACCESS_WRITE; // 0 if no CPU access is necessary.
			bufferDesc.StructureByteStride = sizeof(T_VERTEX);

			D3D11_SUBRESOURCE_DATA vertexBufferData = {
				0
			}; // MS 예제에서 초기화하는 방식
			vertexBufferData.pSysMem		  = vertices;
			vertexBufferData.SysMemPitch	  = 0;
			vertexBufferData.SysMemSlicePitch = 0;

			const HRESULT hr = device->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
			DX::ThrowIfFailed(hr);
		}

		template <typename T_VERTEX>
		static void CreateVertexBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			Common::FTDS::DynamicArray<T_VERTEX>&	  vertices,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer)
		{
			// D3D11_USAGE enumeration (d3d11.h)
			// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.Usage			   = D3D11_USAGE_DYNAMIC;
			bufferDesc.ByteWidth		   = UINT(sizeof(T_VERTEX) * vertices.GetSize());
			bufferDesc.BindFlags		   = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.CPUAccessFlags	   = D3D11_CPU_ACCESS_WRITE; // 0 if no CPU access is necessary.
			bufferDesc.StructureByteStride = sizeof(T_VERTEX);

			D3D11_SUBRESOURCE_DATA vertexBufferData = {
				0
			}; // MS 예제에서 초기화하는 방식
			vertexBufferData.pSysMem		  = vertices.Data();
			vertexBufferData.SysMemPitch	  = 0;
			vertexBufferData.SysMemSlicePitch = 0;

			const HRESULT hr = device->CreateBuffer(&bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
			DX::ThrowIfFailed(hr);
		}

		template <typename T_VERTEX>
		static void CreateVertexBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			UINT								  numVertices,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer)
		{
			D3D11_BUFFER_DESC vertexBufferDesc;
			ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
			vertexBufferDesc.Usage			= D3D11_USAGE_DYNAMIC;			  // Use DYNAMIC to allow CPU writes
			vertexBufferDesc.ByteWidth		= sizeof(T_VERTEX) * numVertices; // Corrected calculation
			vertexBufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			const HRESULT hr = device->CreateBuffer(&vertexBufferDesc, nullptr, vertexBuffer.GetAddressOf());
			DX::ThrowIfFailed(hr);
		}

		template <typename T_CONSTANT>
		static void CreateConstantBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const T_CONSTANT&					  constantBufferData,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& constantBuffer)
		{
			// 주의:
			// For a constant buffer (BindFlags of D3D11_BUFFER_DESC set to
			// D3D11_BIND_CONSTANT_BUFFER), you must set the ByteWidth value of
			// D3D11_BUFFER_DESC in multiples of 16, and less than or equal to
			// D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.

			D3D11_BUFFER_DESC cbDesc;
			cbDesc.ByteWidth		   = sizeof(constantBufferData);
			cbDesc.Usage			   = D3D11_USAGE_DYNAMIC;
			cbDesc.BindFlags		   = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.CPUAccessFlags	   = D3D11_CPU_ACCESS_WRITE;
			cbDesc.MiscFlags		   = 0;
			cbDesc.StructureByteStride = 0;

			// Fill in the subresource data.
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem		  = &constantBufferData;
			initData.SysMemPitch	  = 0;
			initData.SysMemSlicePitch = 0;

			auto hr = device->CreateBuffer(&cbDesc, &initData, constantBuffer.GetAddressOf());
			DX::ThrowIfFailed(hr);
		}

		template <typename T_DATA>
		static void UpdateBuffer(
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
			const T_DATA&								 bufferData,
			Microsoft::WRL::ComPtr<ID3D11Buffer>&		 buffer)
		{
			if (!buffer)
			{
				std::cout << "UpdateBuffer() : buffer was not initialized."
						  << std::endl;
			}

			D3D11_MAPPED_SUBRESOURCE ms;
			DX::ThrowIfFailed(context->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
			memcpy(ms.pData, &bufferData, sizeof(bufferData));
			context->Unmap(buffer.Get(), NULL);
		}

		static void ReadImage(Common::FTDS::String filename, std::vector<uint8_t>& image, int& width, int& height);

		static Microsoft::WRL::ComPtr<ID3D11Texture2D> CreateStagingTexture(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
			const int									 width,
			const int									 height,
			const std::vector<uint8_t>&					 image,
			const int									 mipLevels = 1,
			const int									 arraySize = 1);

		static void
		CreateTextureArray(
			Microsoft::WRL::ComPtr<ID3D11Device>&			  device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>&	  context,
			const std::vector<Common::FTDS::String>			  filenames,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>&		  texture,
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResourceView);

		// static HRESULT CreateCubemapTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device, FTTexture* texture);

		// 텍스춰를 이미지 파일로 저장
		static void WriteToFile(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>&	 textureToWrite,
			Common::FTDS::String&							 filename);

		static UINT GetShaderType(Microsoft::WRL::ComPtr<ID3DBlob>& shaderBlob);
	};
} // namespace D3D11