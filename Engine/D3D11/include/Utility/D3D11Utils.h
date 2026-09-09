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
#include "Debugging/DebugFuncs.h"
#include "Debugging/D3D11DebugFuncs.h"

namespace D3D11
{
	using namespace Common;
	class Camera;
	/// @brief Checks an operation result and reports failure through the debugging UI.
	/// @param hr HRESULT returned by the Direct3D operation.
	/// @param errorBlob Compiler diagnostic blob.
	/// @return Checks an operation result and reports failure through the debugging UI.
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
		/// @brief Creates the Direct3D device and immediate context used by the renderer.
		/// @param window Window used by the operation.
		/// @param device Direct3D device used to create GPU resources.
		/// @param context Context associated with this operation.
		/// @param swapChain Swap chain used to present the window.
		/// @param renderWidth Render-target width.
		/// @param renderHeight Render-target height.
		/// @param numQualityLevel Multisample quality setting.
		/// @return Created device and context instance or resource.
		static HRESULT CreateDeviceAndContext(
			const HWND									 window,
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
			Microsoft::WRL::ComPtr<IDXGISwapChain>&		 swapChain,
			const UINT&									 renderWidth,
			const UINT&									 renderHeight,
			UINT&										 numQualityLevel);

		/// @brief Creates the swap chain used to present rendered frames in the native window.
		/// @param window Window used by the operation.
		/// @param device Direct3D device used to create GPU resources.
		/// @param swapChain Swap chain used to present the window.
		/// @param renderWidth Render-target width.
		/// @param renderHeight Render-target height.
		/// @param numQualityLevel Multisample quality setting.
		/// @return Created swap chain instance or resource.
		static HRESULT CreateSwapChain(
			const HWND								window,
			Microsoft::WRL::ComPtr<ID3D11Device>&	device,
			Microsoft::WRL::ComPtr<IDXGISwapChain>& swapChain,
			int										renderWidth,
			int										renderHeight,
			UINT									numQualityLevel);

		/// @brief Allocates the depth-stencil texture used during rendering.
		/// @param device Direct3D device used to create GPU resources.
		/// @param renderWidth Render-target width.
		/// @param renderHeight Render-target height.
		/// @param numQualityLevels Number of supported multisample quality levels.
		/// @param depthStencilView Depth-stencil view used by the render pass.
		/// @return Created depth buffer instance or resource.
		static HRESULT
		CreateDepthBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>&			device,
			int												renderWidth,
			int												renderHeight,
			UINT											numQualityLevels,
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView);

		/// @brief Creates the vertex shader and input layout from shader bytecode and vertex semantics.
		/// @param device Direct3D device used to create GPU resources.
		/// @param filename Filename used to read or write the resource.
		/// @param inputElements Vertex input-layout descriptors.
		/// @param inputElementsSize Number of vertex input-layout descriptors.
		/// @param vertexShader Vertex shader used by the pipeline.
		/// @param inputLayout Direct3D vertex input layout.
		/// @return Created vertex shader and input layout instance or resource.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		static HRESULT CreateVertexShaderAndInputLayout(
			Microsoft::WRL::ComPtr<ID3D11Device>&		device,
			const std::wstring&							filename,
			const D3D11_INPUT_ELEMENT_DESC*				inputElements,
			size_t										inputElementsSize,
			Microsoft::WRL::ComPtr<ID3D11VertexShader>& vertexShader,
			Microsoft::WRL::ComPtr<ID3D11InputLayout>&	inputLayout);

		/// @brief Creates the vertex shader and input layout from shader bytecode and vertex semantics.
		/// @param device Direct3D device used to create GPU resources.
		/// @param filename Filename used to read or write the resource.
		/// @param inputElements Vertex input-layout descriptors.
		/// @param vertexShader Vertex shader used by the pipeline.
		/// @param inputLayout Direct3D vertex input layout.
		/// @return Created vertex shader and input layout instance or resource.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		static HRESULT CreateVertexShaderAndInputLayout(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			const std::wstring&							 filename,
			const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements,
			Microsoft::WRL::ComPtr<ID3D11VertexShader>&	 vertexShader,
			Microsoft::WRL::ComPtr<ID3D11InputLayout>&	 inputLayout);

		/// @brief Creates the vertex shader and input layout from shader bytecode and vertex semantics.
		/// @param device Direct3D device used to create GPU resources.
		/// @param filename Filename used to read or write the resource.
		/// @param inputElements Vertex input-layout descriptors.
		/// @param vertexShader Vertex shader used by the pipeline.
		/// @param inputLayout Direct3D vertex input layout.
		/// @return Created vertex shader and input layout instance or resource.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		static HRESULT CreateVertexShaderAndInputLayout(
			Microsoft::WRL::ComPtr<ID3D11Device>&						device,
			const std::wstring&											filename,
			const Common::FTDS::DynamicArray<D3D11_INPUT_ELEMENT_DESC>& inputElements,
			Microsoft::WRL::ComPtr<ID3D11VertexShader>&					vertexShader,
			Microsoft::WRL::ComPtr<ID3D11InputLayout>&					inputLayout);

		/// @brief Creates a render-target view used to bind a texture for rendering.
		/// @param RTV Render-target view.
		/// @param device Direct3D device used to create GPU resources.
		/// @param swapChain Swap chain used to present the window.
		/// @return Created render target view instance or resource.
		static HRESULT CreateRenderTargetView(
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& RTV,
			Microsoft::WRL::ComPtr<ID3D11Device>&			device,
			Microsoft::WRL::ComPtr<IDXGISwapChain>&			swapChain);

		/// @brief Creates a render-target view used to bind a texture for rendering.
		/// @param RTV Render-target view.
		/// @param device Direct3D device used to create GPU resources.
		/// @param swapChain Swap chain used to present the window.
		/// @param indexTexture Texture associated with the indexed data.
		/// @param indexTempTexture Temporary texture used while building the resource.
		/// @param indexStagingTexture Staging texture used for CPU access.
		/// @return Created render target view instance or resource.
		static HRESULT CreateRenderTargetView(
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& RTV,
			Microsoft::WRL::ComPtr<ID3D11Device>&			device,
			Microsoft::WRL::ComPtr<IDXGISwapChain>&			swapChain,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>&		indexTexture,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>&		indexTempTexture,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>&		indexStagingTexture);

		/// @brief Creates a Direct3D hull shader from compiled bytecode.
		/// @param device Direct3D device used to create GPU resources.
		/// @param filename Filename used to read or write the resource.
		/// @param hullShader Hull shader used by the pipeline.
		static void CreateHullShader(
			Microsoft::WRL::ComPtr<ID3D11Device>&	  device,
			const std::wstring&						  filename,
			Microsoft::WRL::ComPtr<ID3D11HullShader>& hullShader);

		/// @brief Creates a Direct3D domain shader from compiled bytecode.
		/// @param device Direct3D device used to create GPU resources.
		/// @param filename Filename used to read or write the resource.
		/// @param domainShader Domain shader object used by the pipeline.
		static void CreateDomainShader(
			Microsoft::WRL::ComPtr<ID3D11Device>&		device,
			const std::wstring&							filename,
			Microsoft::WRL::ComPtr<ID3D11DomainShader>& domainShader);

		/// @brief Creates a Direct3D geometry shader from compiled bytecode.
		/// @param device Direct3D device used to create GPU resources.
		/// @param filename Filename used to read or write the resource.
		/// @param geometryShader Geometry shader used by the pipeline.
		static void
		CreateGeometryShader(
			Microsoft::WRL::ComPtr<ID3D11Device>&		  device,
			const std::wstring&							  filename,
			Microsoft::WRL::ComPtr<ID3D11GeometryShader>& geometryShader);

		/// @brief Creates a Direct3D pixel shader from compiled bytecode.
		/// @param device Direct3D device used to create GPU resources.
		/// @param filename Filename used to read or write the resource.
		/// @param pixelShader Pixel shader used by the pipeline.
		/// @return Created pixel shader instance or resource.
		static HRESULT CreatePixelShader(
			Microsoft::WRL::ComPtr<ID3D11Device>&	   device,
			const std::wstring&						   filename,
			Microsoft::WRL::ComPtr<ID3D11PixelShader>& pixelShader);

		/// @brief Creates a GPU index buffer from the supplied index data.
		/// @param device Direct3D device used to create GPU resources.
		/// @param indices Index data defining mesh topology.
		/// @param indexBuffer GPU buffer containing mesh indices.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		static void CreateIndexBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			const std::vector<uint32_t>&		  indices,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer);

		/// @brief Creates a GPU index buffer from the supplied index data.
		/// @param device Direct3D device used to create GPU resources.
		/// @param indices Index data defining mesh topology.
		/// @param indexBuffer GPU buffer containing mesh indices.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		static void CreateIndexBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			Common::FTDS::DynamicArray<uint32_t>& indices,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer);

		/// @brief Creates a GPU index buffer from the supplied index data.
		/// @param device Direct3D device used to create GPU resources.
		/// @param numIndices Number of mesh indices.
		/// @param indexBuffer GPU buffer containing mesh indices.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		static void CreateIndexBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			UINT								  numIndices,
			Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer);

		/// @brief Creates a GPU vertex buffer from the supplied vertex data.
		/// @param device Direct3D device used to create GPU resources.
		/// @param vertices Vertex data defining the mesh.
		/// @param vertexBuffer GPU buffer containing vertices.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
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

		/// @brief Creates a GPU vertex buffer from the supplied vertex data.
		/// @param device Direct3D device used to create GPU resources.
		/// @param vertices Vertex data defining the mesh.
		/// @param vertexCount Number of vertices to upload or draw.
		/// @param vertexBuffer GPU buffer containing vertices.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
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

		/// @brief Creates a GPU vertex buffer from the supplied vertex data.
		/// @param device Direct3D device used to create GPU resources.
		/// @param vertices Vertex data defining the mesh.
		/// @param vertexBuffer GPU buffer containing vertices.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		template <typename T_VERTEX>
		static void CreateVertexBuffer(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			Common::FTDS::DynamicArray<T_VERTEX>& vertices,
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

		/// @brief Creates a GPU vertex buffer from the supplied vertex data.
		/// @param device Direct3D device used to create GPU resources.
		/// @param numVertices Number of mesh vertices.
		/// @param vertexBuffer GPU buffer containing vertices.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
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

		/// @brief Creates a Direct3D constant buffer for shader parameter uploads.
		/// @param device Direct3D device used to create GPU resources.
		/// @param constantBufferData CPU-side shader constants to upload.
		/// @param constantBuffer Direct3D constant buffer used for shader data.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
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

		/// @brief Uploads the current CPU data to its GPU buffer.
		/// @param context Context associated with this operation.
		/// @param bufferData CPU data used to populate the GPU buffer.
		/// @param buffer Buffer used to transfer the data.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
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

		/// @brief Decodes image pixels and creates the texture representation.
		/// @param filename Filename used to read or write the resource.
		/// @param image Image data to load or draw.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		/// @throws std::runtime_error If the operation encounters the failure condition checked by this implementation.
		/// @throws std::length_error If the operation encounters the failure condition checked by this implementation.
		static void ReadImage(Common::FTDS::String filename, std::vector<uint8_t>& image, int& width, int& height);

		/// @brief Creates a CPU-accessible staging texture for transferring image data.
		/// @param device Direct3D device used to create GPU resources.
		/// @param context Context associated with this operation.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		/// @param image Image data to load or draw.
		/// @param mipLevels Number of mip-map levels.
		/// @param arraySize Number of elements in the array.
		/// @return Created staging texture instance or resource.
		static Microsoft::WRL::ComPtr<ID3D11Texture2D> CreateStagingTexture(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
			const int									 width,
			const int									 height,
			const std::vector<uint8_t>&					 image,
			const int									 mipLevels = 1,
			const int									 arraySize = 1);

		/// @brief Creates a texture array and its shader-visible resource view.
		/// @param device Direct3D device used to create GPU resources.
		/// @param context Context associated with this operation.
		/// @param filenames Collection of filenames to process.
		/// @param texture Texture resource used by the operation.
		/// @param textureResourceView Shader resource view for texture sampling.
		static void
		CreateTextureArray(
			Microsoft::WRL::ComPtr<ID3D11Device>&			  device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>&	  context,
			const std::vector<Common::FTDS::String>			  filenames,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>&		  texture,
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureResourceView);

		// static HRESULT CreateCubemapTexture(Microsoft::WRL::ComPtr<ID3D11Device>& device, FTTexture* texture);

		// 텍스춰를 이미지 파일로 저장
		/// @brief Writes the resource's serialized representation to the selected file.
		/// @param device Direct3D device used to create GPU resources.
		/// @param context Context associated with this operation.
		/// @param textureToWrite Texture whose pixels are written to a file.
		/// @param filename Filename used to read or write the resource.
		static void WriteToFile(
			Microsoft::WRL::ComPtr<ID3D11Device>&		 device,
			Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>&	 textureToWrite,
			Common::FTDS::String&						 filename);

		/// @brief Returns the shader type used by this d3 d11 utils.
		/// @param shaderBlob Compiled shader bytecode.
		/// @return Current shader type.
		static UINT GetShaderType(Microsoft::WRL::ComPtr<ID3DBlob>& shaderBlob);
	};
} // namespace D3D11
