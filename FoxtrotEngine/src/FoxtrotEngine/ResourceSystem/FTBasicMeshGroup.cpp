#include "FoxtrotEngine/ResourceSystem/FTBasicMeshGroup.h"

#include "FoxtrotEngine/ResourceSystem/GeometryGenerator.h"
#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"

void FTBasicMeshGroup::Initialize(std::vector<MeshData>& meshes, ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
	D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    device->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());

    mBasicVertexConstantData.model      = DirectX::SimpleMath::Matrix();
    mBasicVertexConstantData.view       = DirectX::SimpleMath::Matrix();
    mBasicVertexConstantData.projection = DirectX::SimpleMath::Matrix();

    D3D11Utils::CreateConstantBuffer(device, mBasicVertexConstantData, mVertexConstantBuffer);
    D3D11Utils::CreateConstantBuffer(device, mBasicPixelConstantData, mPixelConstantBuffer);

    for (const MeshData& meshData : meshes) {
        Mesh* newMesh = new Mesh;
        D3D11Utils::CreateVertexBuffer(device, meshData.vertices,
            newMesh->vertexBuffer);
        newMesh->mIndexCount = UINT(meshData.indices.size());
        newMesh->mVertexCount = UINT(meshData.vertices.size());
        D3D11Utils::CreateIndexBuffer(device, meshData.indices,
            newMesh->indexBuffer);

        //if (!meshData.textureKey.empty()) {

        //    FTTexture* meshTex = ResourceManager::GetInstance()->GetLoadedTexture(meshData.textureKey);
        //    if (meshTex)
        //        newMesh->texture = meshTex;
        //    else
        //        printf("ERROR : FTBasicMeshGroup::Initialize() -> Texture with texture key %s does not exist.", meshData.textureKey.c_str());
        //}

        newMesh->vertexConstantBuffer = mVertexConstantBuffer;
        newMesh->pixelConstantBuffer = mPixelConstantBuffer;

        this->mMeshes.push_back(newMesh);
    }

    vector<D3D11_INPUT_ELEMENT_DESC> basicInputElements = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3 + 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3 + 4 * 3,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    D3D11Utils::CreateVertexShaderAndInputLayout(
        device, VERTEX_SHADER_PATH, basicInputElements,
        mBasicVertexShader, mBasicInputLayout);

    D3D11Utils::CreatePixelShader(device, PIXEL_SHADER_PATH,
        mBasicPixelShader);

    CreateTextureSampler(device);
}

void FTBasicMeshGroup::UpdateConstantBuffers(ComPtr<ID3D11Device>& device, ComPtr<ID3D11DeviceContext>& context)
{
    D3D11Utils::UpdateBuffer(device, context, mBasicVertexConstantData,
        mVertexConstantBuffer);

    D3D11Utils::UpdateBuffer(device, context, mBasicPixelConstantData,
        mPixelConstantBuffer);
}

void FTBasicMeshGroup::Render(ComPtr<ID3D11DeviceContext>& context)
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    for (const Mesh* mesh : mMeshes) {
        context->VSSetShader(mBasicVertexShader.Get(), 0, 0);
        context->VSSetSamplers(0, 1, mSamplerState.GetAddressOf());
        context->VSSetConstantBuffers(0, 1, mesh->vertexConstantBuffer.GetAddressOf());

        context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
        context->PSSetShader(mBasicPixelShader.Get(), 0, 0);

        // 물체 렌더링할 때 큐브맵도 같이 사용
        if (mTexture) {
            std::vector<ID3D11ShaderResourceView*> resViews;
            resViews.push_back(mTexture->GetResourceView().Get());
            context->VSSetShaderResources(0, 1, mTexture->GetResourceView().GetAddressOf());
            context->PSSetShaderResources(0, resViews.size(), resViews.data());
        }
        context->PSSetConstantBuffers(0, 1,
            mesh->pixelConstantBuffer.GetAddressOf());

        context->IASetInputLayout(mBasicInputLayout.Get());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
            &stride, &offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT,
            0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->DrawIndexed(mesh->mIndexCount, 0, 0);
    }
}

void FTBasicMeshGroup::Render(ComPtr<ID3D11DeviceContext>& context, UINT index)
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    for (const Mesh* mesh : mMeshes) {
        context->VSSetShader(mBasicVertexShader.Get(), 0, 0);
        context->VSSetSamplers(0, 1, mSamplerState.GetAddressOf());
        context->VSSetConstantBuffers(0, 1, mesh->vertexConstantBuffer.GetAddressOf());

        context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
        context->PSSetShader(mBasicPixelShader.Get(), 0, 0);

        // 물체 렌더링할 때 큐브맵도 같이 사용
        if (mTexture) {
            std::vector<ID3D11ShaderResourceView*> resViews;
            resViews.push_back(mTexture->GetResourceView().Get());
            context->VSSetShaderResources(0, 1, mTexture->GetResourceView().GetAddressOf());
            context->PSSetShaderResources(0, resViews.size(), resViews.data());
        }
        context->PSSetConstantBuffers(0, 1,
            mesh->pixelConstantBuffer.GetAddressOf());

        context->IASetInputLayout(mBasicInputLayout.Get());
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(),
            &stride, &offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT,
            0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->DrawIndexed(mesh->mIndexCount, 0, 0);
    }
}

void FTBasicMeshGroup::UpdateModelWorld(DirectX::SimpleMath::Matrix& modelToWorldRow)
{
    this->mModelWorldRow = modelToWorldRow;
    this->mInvTransposeRow = modelToWorldRow;
    mInvTransposeRow.Translation(DirectX::SimpleMath::Vector3(0.0f));
    mInvTransposeRow = mInvTransposeRow.Invert().Transpose();

    mBasicVertexConstantData.model = modelToWorldRow.Transpose();
    mBasicVertexConstantData.invTranspose = mInvTransposeRow.Transpose();
}

void FTBasicMeshGroup::SetTexture(std::string key)
{
    if (!mTexture)
        mTexture = new FTTexture;
    mTexture = ResourceManager::GetInstance()->GetLoadedTexture(key);
}

HRESULT FTBasicMeshGroup::CreateTextureSampler(ComPtr<ID3D11Device>& device)
{
    // FTTexture sampler 만들기
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
    : mTexture(new FTTexture)
{
}

FTBasicMeshGroup::~FTBasicMeshGroup()
{
}
