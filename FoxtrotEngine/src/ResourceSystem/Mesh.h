// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Meshes of Foxtrot Engine in DirectX 11 context.
/// </summary>

#pragma once
#include <d3d11.h>
#include <wrl.h> // ComPtr

// #include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Vertex.h"
#include "ResourceSystem/MeshConstantData.h"
#include <Dynamic/DynamicArray.h>

using DirectX::XMFLOAT2;
using DirectX::XMFLOAT3;
using Microsoft::WRL::ComPtr;

struct Mesh
{
	ComPtr<ID3D11Buffer> VertexBuffer;
	ComPtr<ID3D11Buffer> IndexBuffer;

	UINT IndexCount	 = 0;
	UINT VertexCount = 0;
};

enum SPINE_ATTACHMENT_TYPE
{
	SPINE_MESH_ATTACH = 1,
	SPINE_MESH_REGION,
};

struct SpineMesh
{
	enum SPINE_ATTACHMENT_TYPE
	{
		SPINE_MESH_ATTACH = 1,
		SPINE_MESH_REGION,
	};

	int					   DrawOrder;
	int					   MeshType;
	ComPtr<ID3D11Buffer>   PositionBuf;
	ComPtr<ID3D11Buffer>   TexcoordBuf;
	ComPtr<ID3D11Buffer>   IndexBuf;
	UINT				   VertexCount;
	UINT				   IndexCount;
	D3D_PRIMITIVE_TOPOLOGY Primitve{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };

	~SpineMesh()
	{
		PositionBuf.Reset();
		TexcoordBuf.Reset();
		IndexBuf.Reset();
		VertexCount = 0;
		IndexCount	= 0;
	}

	int ResourceBinding(
		ComPtr<ID3D11Device>& device,
		int					  order,
		SPINE_ATTACHMENT_TYPE attachmentType,
		size_t				  vertexCount,
		size_t				  indexCount)
	{
		HRESULT hr = S_OK;
		if (!device)
			return E_FAIL;

		if (0 >= vertexCount)
			return E_FAIL;

		this->DrawOrder	  = order;
		this->MeshType	  = attachmentType;
		this->VertexCount = static_cast<UINT>(vertexCount);
		this->IndexCount  = static_cast<UINT>(indexCount);
		// vertex buffer
		{
			// position
			D3D11_BUFFER_DESC ibDesc = {};
			ZeroMemory(&ibDesc, sizeof(ibDesc));
			ibDesc.Usage		  = D3D11_USAGE_DYNAMIC;
			ibDesc.ByteWidth	  = static_cast<UINT>(vertexCount * sizeof(XMFLOAT2));
			ibDesc.BindFlags	  = D3D11_BIND_VERTEX_BUFFER;
			ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			hr					  = device->CreateBuffer(&ibDesc, nullptr, PositionBuf.GetAddressOf());
			if (FAILED(hr))
				return hr;

			// texture coord
			ibDesc.ByteWidth = static_cast<UINT>(vertexCount * sizeof(XMFLOAT2));
			hr				 = device->CreateBuffer(&ibDesc, nullptr, TexcoordBuf.GetAddressOf());
			if (FAILED(hr))
				return hr;
		}
		// index buffer
		if (indexCount)
		{
			D3D11_BUFFER_DESC ibDesc = {};
			ibDesc.Usage			 = D3D11_USAGE_DYNAMIC;
			ibDesc.ByteWidth		 = static_cast<UINT>(indexCount * sizeof(uint16_t));
			ibDesc.BindFlags		 = D3D11_BIND_INDEX_BUFFER;
			ibDesc.CPUAccessFlags	 = D3D11_CPU_ACCESS_WRITE;
			hr						 = device->CreateBuffer(&ibDesc, nullptr, &IndexBuf);
			if (FAILED(hr))
				return hr;

			Primitve = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
		return S_OK;
	}

	void Draw(ComPtr<ID3D11DeviceContext>& context)
	{
		ID3D11Buffer* buffers[] = { PositionBuf.Get(), TexcoordBuf.Get() };
		UINT		  strides[] = { (UINT)sizeof(float) * 2, (UINT)sizeof(float) * 2 };
		UINT		  offsets[] = { 0, 0 };
		context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
		context->IASetPrimitiveTopology(Primitve);
		if (IndexCount)
		{
			context->IASetIndexBuffer(IndexBuf.Get(), DXGI_FORMAT_R16_UINT, 0);
			context->DrawIndexed(IndexCount, 0, 0);
		}
		else
		{
			context->Draw(VertexCount, 0);
		}
	}
};