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
#include "Renderer/FoxtrotRenderer.h"

void FTBasicMeshGroup::Initialize(std::vector<MeshData>& meshes, ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
    CreateTextureSampler(device);
    InitializeMeshes(device, meshes);
    InitializeConstantBuffer(device);
}

void FTBasicMeshGroup::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
    for (Mesh* mesh : mMeshes)
    {
        if (mesh)
        {
            D3D11Utils::UpdateBuffer(
                device, context, mesh->VertexConstantData,
                mesh->VertexConstantBuffer
            );

            D3D11Utils::UpdateBuffer(
                device, context, mesh->PixelConstantData,
                mesh->PixelConstantBuffer
            );
        }
    }
}

void FTBasicMeshGroup::Render(FoxtrotRenderer* renderer, FTTexture* texture)
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

    for (const Mesh* mesh : mMeshes) 
    {
        context->VSSetSamplers(0, 1, mSamplerState.GetAddressOf());
        context->VSSetConstantBuffers(0, 1, mesh->VertexConstantBuffer.GetAddressOf());

        context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

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
            context->VSSetShader(renderer->GetSolidVS().Get(), 0, 0);
            context->PSSetShader(renderer->GetSolidPS().Get(), 0, 0);
            context->IASetInputLayout(renderer->GetSolidInputLayout().Get());
        }

        context->PSSetConstantBuffers(0, 1,
            mesh->PixelConstantBuffer.GetAddressOf());

        context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(),
            &stride, &offset);
        context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT,
            0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->DrawIndexed(mesh->IndexCount, 0, 0);
    }
}

void FTBasicMeshGroup::Render(FoxtrotRenderer* renderer, FTTexture* texture, int meshIndex)
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    Mesh* mesh = mMeshes.at(meshIndex);
    ComPtr<ID3D11DeviceContext>& context = renderer->GetContext();

    if (mesh) 
    {
        context->VSSetSamplers(0, 1, mSamplerState.GetAddressOf());
        context->VSSetConstantBuffers(0, 1, mesh->VertexConstantBuffer.GetAddressOf());

        context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

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
            context->VSSetShader(renderer->GetSolidVS().Get(), 0, 0);
            context->PSSetShader(renderer->GetSolidPS().Get(), 0, 0);
            context->IASetInputLayout(renderer->GetSolidInputLayout().Get());
        }

        context->PSSetConstantBuffers(0, 1,
            mesh->PixelConstantBuffer.GetAddressOf());

        context->IASetVertexBuffers(0, 1, mesh->VertexBuffer.GetAddressOf(),
            &stride, &offset);
        context->IASetIndexBuffer(mesh->IndexBuffer.Get(), DXGI_FORMAT_R32_UINT,
            0);
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
}

ComPtr<ID3D11SamplerState>& FTBasicMeshGroup::GetSamplerState()
{
    return mSamplerState;
}

int FTBasicMeshGroup::GetMeshCount()
{
    return mMeshes.size();
}

std::vector<Mesh*>& FTBasicMeshGroup::GetMeshes()
{
    return mMeshes;
}

void FTBasicMeshGroup::InitializeMeshes(ComPtr<ID3D11Device>& device, std::vector<MeshData>& meshes)
{
    if (0 < meshes.size())
        Clear();

    for (const MeshData& meshData : meshes) {
        Mesh* newMesh = DBG_NEW Mesh;
        newMesh->IndexCount = UINT(meshData.indices.size());
        newMesh->VertexCount = UINT(meshData.vertices.size());

        D3D11Utils::CreateVertexBuffer(device, meshData.vertices,
            newMesh->VertexBuffer);
        D3D11Utils::CreateIndexBuffer(device, meshData.indices,
            newMesh->IndexBuffer);

        this->mMeshes.push_back(newMesh);
    }
}

void FTBasicMeshGroup::InitializeConstantBuffer(ComPtr<ID3D11Device>& device)
{
    for (Mesh* mesh : mMeshes) 
    {
        D3D11Utils::CreateConstantBuffer(device, mesh->VertexConstantData, mesh->VertexConstantBuffer);
        D3D11Utils::CreateConstantBuffer(device, mesh->PixelConstantData, mesh->PixelConstantBuffer);
    }
}

HRESULT FTBasicMeshGroup::CreateTextureSampler(ComPtr<ID3D11Device>& device)
{
    // FTTexture sampler ¸¸µé±â
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the Sample State
    return device->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());
}

FTBasicMeshGroup::FTBasicMeshGroup()
    : FTResource()
    , mMeshes()
{}

FTBasicMeshGroup::~FTBasicMeshGroup()
{
    Clear();
}
