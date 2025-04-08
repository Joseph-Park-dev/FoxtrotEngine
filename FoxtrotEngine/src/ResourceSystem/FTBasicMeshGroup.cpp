// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/FTBasicMeshGroup.h"

#include <vector>

#include "ResourceSystem/GeometryGenerator.h"
#include "ResourceSystem/FTMaterials/FTMaterial.h"
#include "Managers/ResourceManager.h"
#include "InputSystem/FTInputDevice.h"
#include "Renderer/Camera.h"
#include "Renderer/FoxtrotRenderer.h"

using Matrix = DirectX::SimpleMath::Matrix;

void FTBasicMeshGroup::Initialize(
	std::vector<FTMeshData>&&	 meshes,
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

	for (Mesh* mesh : mMeshes)
	{
		size_t bufferCount = 0;
		for (FTMaterial* mat : mMaterials)
		{
			mat->UpdateBuffer(context, mesh->PixelConstantBuffers.at(bufferCount));
			++bufferCount;
		}
	}

#ifdef FOXTROT_EDITOR
	if (mDrawNormal && mValModified)
	{
		D3D11Utils::UpdateBuffer(
			context, mNormalVertexConstData, mNormalLines->VertexConstantBuffers.at(0));
		mValModified = false;
	}
#endif // FOXTROT_EDITOR
}

void FTBasicMeshGroup::Render(FoxtrotRenderer* renderer)
{
	UINT						 stride	 = sizeof(Vertex);
	UINT						 offset	 = 0;
	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	for (const Mesh* mesh : mMeshes)
	{
		context->VSSetConstantBuffers(
			0, mesh->VertexConstantBuffers.size(), mesh->VertexConstantBuffers.data()->GetAddressOf());

		mVS = renderer->GetTextureVS();
		mPS = renderer->GetRimTexturePS();

		if (mTexture)
		{
			std::vector<ID3D11ShaderResourceView*> resViews;
			resViews.push_back(mTexture->GetResourceView().Get());
			context->PSSetShaderResources(0, (UINT)resViews.size(), resViews.data());
		}

		context->VSSetShader(mVS.Get(), 0, 0);
		context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
		context->PSSetShader(mPS.Get(), 0, 0);

		if (!mMaterials.empty())
		{
			context->PSSetConstantBuffers(
				0, mesh->PixelConstantBuffers.size(), mesh->PixelConstantBuffers.data()->GetAddressOf());
		}

		context->IASetInputLayout(renderer->GetTextureInputLayout().Get());
		context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->OMSetBlendState(renderer->GetBlendState().Get(), NULL, D3D11_DEFAULT_SAMPLE_MASK);

		context->DrawIndexed(mesh->IndexCount, 0, 0);
	}

	if (mDrawNormal)
	{
		context->VSSetShader(renderer->GetNormalVS().Get(), 0, 0);
		context->PSSetShader(renderer->GetNormalPS().Get(), 0, 0);
		ID3D11Buffer* pptr[2] = { mVertexConstBuffer.Get(), mNormalLines->VertexConstantBuffers.at(0).Get() };
		context->VSSetConstantBuffers(0, 2, pptr);
		context->IASetVertexBuffers(0, 1, mNormalLines->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mNormalLines->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		context->DrawIndexed(mNormalLines->IndexCount, 0, 0);
	}
}

void FTBasicMeshGroup::Render(FoxtrotRenderer* renderer, int meshIndex)
{
	UINT						 stride	 = sizeof(Vertex);
	UINT						 offset	 = 0;
	Mesh*						 mesh	 = mMeshes.at(meshIndex);
	ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

	if (mesh)
	{
		context->VSSetConstantBuffers(
			0, mesh->VertexConstantBuffers.size(), mesh->VertexConstantBuffers.data()->GetAddressOf());

		mVS = renderer->GetTextureVS();
		mPS = renderer->GetRimTexturePS();

		if (mTexture)
		{
			std::vector<ID3D11ShaderResourceView*> resViews;
			resViews.push_back(mTexture->GetResourceView().Get());
			context->PSSetShaderResources(0, (UINT)resViews.size(), resViews.data());
		}

		context->VSSetShader(mVS.Get(), 0, 0);
		context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
		context->PSSetShader(mPS.Get(), 0, 0);

		if (!mMaterials.empty())
		{
			context->PSSetConstantBuffers(
				0, mesh->PixelConstantBuffers.size(), mesh->PixelConstantBuffers.data()->GetAddressOf());
		}

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
		ID3D11Buffer* pptr[2] = { mVertexConstBuffer.Get(), mNormalLines->VertexConstantBuffers.at(0).Get() };
		context->VSSetConstantBuffers(0, 2, pptr);
		context->IASetVertexBuffers(0, 1, mNormalLines->VertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mNormalLines->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		context->DrawIndexed(mNormalLines->IndexCount, 0, 0);
	}
}

void FTBasicMeshGroup::Clear()
{
	mMaterials.clear();
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
size_t						FTBasicMeshGroup::GetMeshCount() { return mMeshes.size(); }
FTTexture*					FTBasicMeshGroup::GetTexture() const { return mTexture; }
BasicVCData&				FTBasicMeshGroup::GetVCData() { return mVertexConstData; }
bool						FTBasicMeshGroup::GetDrawNormal() { return mDrawNormal; }

std::vector<Mesh*>&		  FTBasicMeshGroup::Meshes() { return mMeshes; }
std::vector<FTMaterial*>& FTBasicMeshGroup::Materials() { return mMaterials; }
Mesh*					  FTBasicMeshGroup::NormalLines() { return mNormalLines; }

void FTBasicMeshGroup::SetMaterials(std::vector<UINT>& matKeys, ComPtr<ID3D11Device>& device)
{
	if(0 < mMaterials.size())
		mMaterials.clear();

	for (Mesh* mesh : mMeshes)
		mesh->PixelConstantBuffers.clear();

	if (matKeys.empty())
	{
		Debug::LogError(__LINE__, __FILE__, "Material Key not assigned");
		return;
	}
	for (UINT& key : matKeys)
		mMaterials.push_back(ResourceManager::GetInstance()->GetLoadedMaterial(key));

	for (Mesh* mesh : mMeshes)
	{
		mesh->PixelConstantBuffers.reserve(mMaterials.size());
		for (FTMaterial* mat : mMaterials)
		{
			ComPtr<ID3D11Buffer> pixelConstBuffer;
			mat->CreatePixelConstBuffer(device, pixelConstBuffer);
			mesh->PixelConstantBuffers.push_back(pixelConstBuffer);
		}
	}
}

void FTBasicMeshGroup::SetTexture()
{
	if (mTexKey == ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	{
		printf("ERROR: MeshRenderer::SetTexture() -> TexKey not assigned.\n");
		return;
	}
	mTexture = ResourceManager::GetInstance()->GetLoadedTexture(mTexKey);
	if (!mTexture)
		printf("ERROR: MeshRenderer::SetTexture() -> Cannot set texture %d, returning nullptr.\n", mTexKey);
}

void FTBasicMeshGroup::SetTexture(UINT texKey)
{
	mTexKey = texKey;
	SetTexture();
}

void FTBasicMeshGroup::SetNormalLines(Mesh* normalLines) { mNormalLines = normalLines; }
void FTBasicMeshGroup::SetDrawNormal(bool drawNormal) { mDrawNormal = drawNormal; }

void FTBasicMeshGroup::InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<FTMeshData>& meshes)
{
	if (0 < meshes.size())
		Clear();

	mMeshes.reserve(meshes.size());
	for (const FTMeshData& meshData : meshes)
	{
		Mesh* newMesh		 = DBG_NEW Mesh;
		newMesh->VertexCount = UINT(meshData.Vertices.size());
		newMesh->IndexCount	 = UINT(meshData.Indices.size());

		D3D11Utils::CreateVertexBuffer(device, meshData.Vertices, newMesh->VertexBuffer);
		D3D11Utils::CreateIndexBuffer(device, meshData.Indices, newMesh->IndexBuffer);

		this->mMeshes.push_back(newMesh);
	}

	mNormalLines = DBG_NEW Mesh;
	std::vector<Vertex>	   normalVertices;
	std::vector<uint32_t>  normalIndices;

	size_t offset = 0;
	for (const FTMeshData& meshData : meshes)
	{
		for (size_t i = 0; i < meshData.Vertices.size(); ++i)
		{
			Vertex v = meshData.Vertices.at(i);

			v.texcoord.x = 0.0f; // start point
			normalVertices.push_back(v);

			v.texcoord.x = 1.0f; // end point
			normalVertices.push_back(v);

			normalIndices.push_back(uint32_t(2 * (i + offset)));
			normalIndices.push_back(uint32_t(2 * (i + offset) + 1));
		}
		offset = meshData.Vertices.size();
	}

	D3D11Utils::CreateVertexBuffer<Vertex>(device, normalVertices, mNormalLines->VertexBuffer);
	mNormalLines->VertexCount = (UINT)normalVertices.size();
	D3D11Utils::CreateIndexBuffer(device, normalIndices, mNormalLines->IndexBuffer);
	mNormalLines->IndexCount = (UINT)normalIndices.size();
}

void FTBasicMeshGroup::InitializeConstantBuffers(ComPtr<ID3D11Device>& device)
{
	D3D11Utils::CreateConstantBuffer(device, mVertexConstData, mVertexConstBuffer);

	for (Mesh* mesh : mMeshes)
	{
		mesh->VertexConstantBuffers.push_back(mVertexConstBuffer);

		for (FTMaterial* mat : mMaterials)
		{
			ComPtr<ID3D11Buffer> pixelConstBuffer;
			mat->CreatePixelConstBuffer(device, pixelConstBuffer);
			mesh->PixelConstantBuffers.push_back(pixelConstBuffer);
		}
	}

	if (mNormalLines)
	{
		ComPtr<ID3D11Buffer> normalConstBuf;
		D3D11Utils::CreateConstantBuffer(device, mNormalVertexConstData, normalConstBuf);
		mNormalLines->VertexConstantBuffers.push_back(normalConstBuf);
	}
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
	: mTexKey(ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	, mMeshes()
	, mTexture(nullptr)
	, mNormalLines(nullptr)
	, mDrawNormal(false)
#ifdef FOXTROT_EDITOR
	, mValModified(false)
#endif // FOXTROT_EDITOR
{
}

FTBasicMeshGroup::FTBasicMeshGroup(FTMeshData meshData, FoxtrotRenderer* renderer)
	: mTexKey(ChunkKey::NullVal::VALUE_NOT_ASSIGNED)
	, mMeshes()
	, mTexture(nullptr)
	, mNormalLines(nullptr)
	, mDrawNormal(false)
#ifdef FOXTROT_EDITOR
	, mValModified(false)
#endif // FOXTROT_EDITOR
{
	Initialize({ meshData }, renderer->GetDevice(), renderer->GetContext());
}

FTBasicMeshGroup::~FTBasicMeshGroup()
{
	Clear();
}

void FTBasicMeshGroup::SaveProperties(std::ofstream& ofs, UINT key)
{
	FTResource::SaveProperties(ofs, key);
	FileIOHelper::SaveUnsignedInt(ofs, ChunkKey::FTMeshGroup::TEXTURE_KEY, mTexKey);
}

UINT FTBasicMeshGroup::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::LoadUnsignedInt(ifs, mTexKey);
	return FTResource::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void FTBasicMeshGroup::UpdateUI()
{
	CommandHistory::GetInstance()->UpdateBoolValue("Draw Normal", mDrawNormal);

	if (ImGui::DragFloat("Normal Scale", &mNormalVertexConstData.scale, FLOATMOD_SPEED))
	{
		mValModified = true;
	}
}
#endif // FOXTROT_EDITOR