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
#include <wrl.h>
#include <D3D11.h>

#include "FTDS/Dynamic/DynamicArray.h"
#include "FTMath.h"

namespace D3D11
{
	struct Mesh
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> VertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuffer;

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
		int									 DrawOrder;
		int									 MeshType;
		Microsoft::WRL::ComPtr<ID3D11Buffer> PositionBuf;
		Microsoft::WRL::ComPtr<ID3D11Buffer> TexcoordBuf;
		Microsoft::WRL::ComPtr<ID3D11Buffer> IndexBuf;
		UINT								 VertexCount;
		UINT								 IndexCount;
		D3D_PRIMITIVE_TOPOLOGY				 Primitve{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP };

		/// @brief Completes destruction through the object's inheritance hierarchy.
		~SpineMesh()
		{
			PositionBuf.Reset();
			TexcoordBuf.Reset();
			IndexBuf.Reset();
			VertexCount = 0;
			IndexCount	= 0;
		}

		/// @brief Binds the resource views and GPU state required for drawing.
		/// @param device Direct3D device used to create GPU resources.
		/// @param order Ordering value used when processing or drawing the object.
		/// @param attachmentType Spine attachment category.
		/// @param vertexCount Number of vertices to upload or draw.
		/// @param indexCount Number of indices to upload or draw.
		/// @return Binds the resource views and GPU state required for drawing.
		int ResourceBinding(
			Microsoft::WRL::ComPtr<ID3D11Device>& device,
			int									  order,
			SPINE_ATTACHMENT_TYPE				  attachmentType,
			size_t								  vertexCount,
			size_t								  indexCount)
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
				ibDesc.ByteWidth	  = static_cast<UINT>(vertexCount * sizeof(Math::FTVector2));
				ibDesc.BindFlags	  = D3D11_BIND_VERTEX_BUFFER;
				ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				hr					  = device->CreateBuffer(&ibDesc, nullptr, PositionBuf.GetAddressOf());
				if (FAILED(hr))
					return hr;

				// texture coord
				ibDesc.ByteWidth = static_cast<UINT>(vertexCount * sizeof(Math::FTVector2));
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

		/// @brief Submits geometry or text to the configured rendering target.
		/// @param context Context associated with this operation.
		void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
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
} // namespace D3D11
