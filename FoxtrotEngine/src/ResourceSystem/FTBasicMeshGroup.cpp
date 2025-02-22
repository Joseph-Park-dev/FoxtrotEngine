// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/FTBasicMeshGroup.h"

#include "ResourceSystem/GeometryGenerator.h"
#include "Managers/ResourceManager.h"
#include "Renderer/Camera.h"
#include "Renderer/FoxtrotRenderer.h"

using Matrix = DirectX::SimpleMath::Matrix;

void FTBasicMeshGroup::Initialize(
	std::vector<MeshData>&		 meshes,
	ComPtr<ID3D11Device>&		 device,
	ComPtr<ID3D11DeviceContext>& context)
{
	CreateTextureSampler(device);
	InitializeMeshes(device, meshes);
	InitializeConstantBuffers(device);
}

void FTBasicMeshGroup::UpdateConstantBuffers(
	ComPtr<ID3D11Device>&		 device,
	ComPtr<ID3D11DeviceContext>& context)
{
	D3D11Utils::UpdateBuffer(
		context, mVertexConstData, mVertexConstBuffer);

	D3D11Utils::UpdateBuffer(
		context, mPixelConstData, mPixelConstBuffer);

#ifdef FOXTROT_EDITOR
	if (mDrawNormal && mValModified)
	{
		D3D11Utils::UpdateBuffer(
			context, mNormalVertexConstData, mVertexConstBuffer);
		mValModified = false;
	}
#endif // FOXTROT_EDITOR
}

void FTBasicMeshGroup::Render(FoxtrotRenderer* renderer, FTTexture* texture)
{
	UINT						 stride	 = sizeof(Vertex);
	UINT						 offset	 = 0;
	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	context->VSSetShader(renderer->GetTextureVS().Get(), 0, 0);
	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	context->PSSetShader(renderer->GetTexturePS().Get(), 0, 0);

	for (const Mesh* mesh : mMeshes)
	{
		context->VSSetConstantBuffers(0, 1, mesh->VertexConstantBuffer.GetAddressOf());
		
		if (texture)
		{
			std::vector<ID3D11ShaderResourceView*> resViews;
			resViews.push_back(texture->GetResourceView().Get());
			context->PSSetShaderResources(0, resViews.size(), resViews.data());
		}
		context->PSSetConstantBuffers(0, 1, mesh->PixelConstantBuffer.GetAddressOf());

		context->IASetInputLayout(renderer->GetTextureInputLayout().Get());
		context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(mesh->IndexCount, 0, 0);
	}

	if (mDrawNormal)
	{
		context->VSSetShader(renderer->GetNormalVS().Get(), 0, 0);
		context->PSSetShader(renderer->GetNormalPS().Get(), 0, 0);
		ID3D11Buffer* pptr[2] = { mVertexConstBuffer.Get(), mNormalLines->VertexBuffer.Get() };
		context->VSSetConstantBuffers(0, 2, pptr);
		context->IASetVertexBuffers(0, 1, mNormalLines->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mNormalLines->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexed(mNormalLines->IndexCount, 0, 0);
	}
}

void FTBasicMeshGroup::Render(FoxtrotRenderer* renderer, FTTexture* texture, int meshIndex)
{
	UINT						 stride	 = sizeof(Vertex);
	UINT						 offset	 = 0;
	Mesh*						 mesh	 = mMeshes.at(meshIndex);
	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	if (mesh)
	{
		context->VSSetSamplers(0, 1, mSamplerState.GetAddressOf());
		context->VSSetConstantBuffers(0, 1, mesh->VertexConstantBuffer.GetAddressOf());

		context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

		if (mDrawTexture)
		{
			if (texture)
			{
				context->VSSetShader(renderer->GetTextureVS().Get(), 0, 0);
				context->PSSetShader(renderer->GetTexturePS().Get(), 0, 0);
				context->IASetInputLayout(renderer->GetTextureInputLayout().Get());

				std::vector<ID3D11ShaderResourceView*> resViews;
				resViews.push_back(texture->GetResourceView().Get());
				context->VSSetShaderResources(0, 1, texture->GetResourceView().GetAddressOf());
				context->PSSetShaderResources(0, resViews.size(), resViews.data());
			}
			else
			{
				Debug::LogError(__LINE__, __FILE__, "Texture is NULL");
			}
		}
		else
		{
			context->VSSetShader(renderer->GetSolidVS().Get(), 0, 0);
			context->PSSetShader(renderer->GetSolidPS().Get(), 0, 0);
			context->IASetInputLayout(renderer->GetSolidInputLayout().Get());
		}

		context->PSSetConstantBuffers(0, 1, mesh->PixelConstantBuffer.GetAddressOf());

		context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->DrawIndexed(mesh->IndexCount, 0, 0);
	}
}

void FTBasicMeshGroup::Clear()
{
	for (Mesh* mesh : mMeshes)
	{
		if (mesh)
		{
			delete mesh;
			mesh = nullptr;
		}
	}
	mMeshes.clear();
	delete mNormalLines;
}

ComPtr<ID3D11SamplerState>& FTBasicMeshGroup::GetSamplerState() { return mSamplerState; }
int							FTBasicMeshGroup::GetMeshCount() { return mMeshes.size(); }
std::vector<Mesh*>&			FTBasicMeshGroup::GetMeshes() { return mMeshes; }

BasicVCData& FTBasicMeshGroup::GetVCData() { return mVertexConstData; }
BasicPCData& FTBasicMeshGroup::GetPCData() { return mPixelConstData; }

bool FTBasicMeshGroup::GetDrawTexture() { return mDrawTexture; }

void FTBasicMeshGroup::InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<MeshData>& meshes)
{
	if (0 < meshes.size())
		Clear();

	for (const MeshData& meshData : meshes)
	{
		Mesh* newMesh		 = DBG_NEW Mesh;
		newMesh->VertexCount = UINT(meshData.vertices.size());
		newMesh->IndexCount	 = UINT(meshData.indices.size());

		D3D11Utils::CreateVertexBuffer(device, meshData.vertices, newMesh->VertexBuffer);
		D3D11Utils::CreateIndexBuffer(device, meshData.indices, newMesh->IndexBuffer);

		this->mMeshes.push_back(newMesh);
	}

	mNormalLines = new Mesh;
	std::vector<Vertex>	  normalVertices;
	std::vector<uint32_t> normalIndices;

	size_t offset = 0;
	for (const MeshData& meshData : meshes)
	{
		for (size_t i = 0; i < meshData.vertices.size(); ++i)
		{
			Vertex v = meshData.vertices.at(i);

			v.texcoord.x = 0.0f; // start point
			normalVertices.push_back(v);

			v.texcoord.x = 1.0f; // end point
			normalVertices.push_back(v);

			normalIndices.push_back(uint32_t(2 * (i + offset)));
			normalIndices.push_back(uint32_t(2 * (i + offset) + 1));
		}
		offset = meshData.vertices.size();
	}

	D3D11Utils::CreateVertexBuffer<Vertex>(device, normalVertices, mNormalLines->VertexBuffer);
	mNormalLines->VertexCount = (UINT)normalVertices.size();
	D3D11Utils::CreateIndexBuffer(device, normalIndices, mNormalLines->IndexBuffer);
	mNormalLines->IndexCount = (UINT)normalIndices.size();
}

void FTBasicMeshGroup::InitializeConstantBuffers(ComPtr<ID3D11Device>& device)
{
	D3D11Utils::CreateConstantBuffer(device, mVertexConstData, mVertexConstBuffer);
	D3D11Utils::CreateConstantBuffer(device, mPixelConstData, mPixelConstBuffer);

	for (Mesh* mesh : mMeshes)
	{
		mesh->VertexConstantBuffer = mVertexConstBuffer;
		mesh->PixelConstantBuffer  = mPixelConstBuffer;
	}
	D3D11Utils::CreateConstantBuffer(device, mNormalVertexConstData, mNormalLines->VertexConstantBuffer);
}

HRESULT FTBasicMeshGroup::CreateTextureSampler(ComPtr<ID3D11Device>& device)
{
	// FTTexture sampler ¸¸µé±â
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD			= 0;
	sampDesc.MaxLOD			= D3D11_FLOAT32_MAX;

	// Create the Sample State
	return device->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());
}

FTBasicMeshGroup::FTBasicMeshGroup()
	: FTResource()
	, mMeshes()
	, mNormalLines(nullptr)
	, mDrawTexture(true)
#ifdef FOXTROT_EDITOR
	, mNormalScale(0.1f)
	, mDrawNormal(false)
	, mValModified(false)
#endif // FOXTROT_EDITOR
{
}

FTBasicMeshGroup::~FTBasicMeshGroup()
{
	Clear();
}

#ifdef FOXTROT_EDITOR
void FTBasicMeshGroup::UpdateUI()
{
	CommandHistory::GetInstance()->UpdateBoolValue("Draw Texture", mDrawTexture);
	mPixelConstData.UseTexture = mDrawTexture;

	CommandHistory::GetInstance()->UpdateBoolValue("Draw Normal", mDrawNormal);

	CommandHistory::GetInstance()->UpdateFloatValue("Normal Scale", &mNormalScale);
}
#endif // FOXTROT_EDITOR
