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
#include <wrl/client.h> // ComPtr

#include <ResourceSystem/FTTexture.h>
#include "Core/TemplateFunctions.h"

class Camera;

using Microsoft::WRL::ComPtr;
using std::vector;
using std::wstring;

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
		const HWND					 window,
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context,
		ComPtr<IDXGISwapChain>&		 swapChain,
		const UINT&					 renderWidth,
		const UINT&					 renderHeight,
		UINT&						 numQualityLevel);

	static HRESULT
	CreateDepthBuffer(
		ComPtr<ID3D11Device>&			device,
		int								renderWidth,
		int								renderHeight,
		UINT							numQualityLevels,
		ComPtr<ID3D11DepthStencilView>& depthStencilView);

	static HRESULT CreateVertexShaderAndInputLayout(
		ComPtr<ID3D11Device>&					device,
		const wstring&							filename,
		const vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
		ComPtr<ID3D11VertexShader>&				vertexShader,
		ComPtr<ID3D11InputLayout>&				inputLayout);

	static HRESULT CreateRenderTargetView(
		ComPtr<ID3D11RenderTargetView>& RTV,
		ComPtr<ID3D11Device>&			device,
		ComPtr<IDXGISwapChain>&			swapChain);

	static HRESULT CreateRenderTargetView(
		ComPtr<ID3D11RenderTargetView>& RTV, 
		ComPtr<ID3D11Device>& device, 
		ComPtr<IDXGISwapChain>& swapChain, 
		ComPtr<ID3D11Texture2D>& indexTexture, 
		ComPtr<ID3D11Texture2D>& indexTempTexture, 
		ComPtr<ID3D11Texture2D>& indexStagingTexture);

	static void CreateHullShader(
		ComPtr<ID3D11Device>&	  device,
		const wstring&			  filename,
		ComPtr<ID3D11HullShader>& hullShader);

	static void CreateDomainShader(
		ComPtr<ID3D11Device>&		device,
		const wstring&				filename,
		ComPtr<ID3D11DomainShader>& domainShader);

	static void
	CreateGeometryShader(
		ComPtr<ID3D11Device>&		  device,
		const wstring&				  filename,
		ComPtr<ID3D11GeometryShader>& geometryShader);

	static HRESULT CreatePixelShader(
		ComPtr<ID3D11Device>&	   device,
		const wstring&			   filename,
		ComPtr<ID3D11PixelShader>& pixelShader);

	static void CreateIndexBuffer(
		ComPtr<ID3D11Device>&	device,
		const vector<uint32_t>& indices,
		ComPtr<ID3D11Buffer>&	indexBuffer);

	static void CreateIndexBuffer(
		ComPtr<ID3D11Device>& device,
		UINT				  numIndices,
		ComPtr<ID3D11Buffer>& indexBuffer);

	template <typename T_VERTEX>
	static void CreateVertexBuffer(
		ComPtr<ID3D11Device>&	device,
		const vector<T_VERTEX>& vertices,
		ComPtr<ID3D11Buffer>&	vertexBuffer)
	{
		// D3D11_USAGE enumeration (d3d11.h)
		// https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.Usage			   = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
		bufferDesc.ByteWidth		   = UINT(sizeof(T_VERTEX) * vertices.size());
		bufferDesc.BindFlags		   = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags	   = 0; // 0 if no CPU access is necessary.
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
		ComPtr<ID3D11Device>& device,
		UINT				  numVertices,
		ComPtr<ID3D11Buffer>& vertexBuffer)
	{
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		vertexBufferDesc.Usage			= D3D11_USAGE_IMMUTABLE;		  // Use DYNAMIC to allow CPU writes
		vertexBufferDesc.ByteWidth		= sizeof(T_VERTEX) * numVertices; // Corrected calculation
		vertexBufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;

		const HRESULT hr = device->CreateBuffer(&vertexBufferDesc, nullptr, vertexBuffer.GetAddressOf());
		DX::ThrowIfFailed(hr);
	}

	template <typename T_CONSTANT>
	static void CreateConstantBuffer(
		ComPtr<ID3D11Device>& device,
		const T_CONSTANT&	  constantBufferData,
		ComPtr<ID3D11Buffer>& constantBuffer)
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
		ComPtr<ID3D11DeviceContext>& context,
		const T_DATA&				 bufferData,
		ComPtr<ID3D11Buffer>&		 buffer)
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

	static void
	CreateTexture(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context,
		FTTexture*					 texture);

	static void
	CreateTextureArray(
		ComPtr<ID3D11Device>&			  device,
		ComPtr<ID3D11DeviceContext>&	  context,
		const std::vector<std::string>	  filenames,
		ComPtr<ID3D11Texture2D>&		  texture,
		ComPtr<ID3D11ShaderResourceView>& textureResourceView);

	static void
	CreateCubemapTexture(
		ComPtr<ID3D11Device>&			  device,
		const wchar_t*					  filename,
		ComPtr<ID3D11ShaderResourceView>& texResView);

	// 텍스춰를 이미지 파일로 저장
	static void WriteToFile(
		ComPtr<ID3D11Device>&		 device,
		ComPtr<ID3D11DeviceContext>& context,
		ComPtr<ID3D11Texture2D>&	 textureToWrite,
		const std::string			 filename);
};