#include <FTTexture.h>

#include <string>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <d3d11.h>
#include <wrl.h> // ComPtr

#include <FTCore.h>
#include <TemplateFunctions.h>

bool FTTexture::CreateTexture(FoxtrotRenderer* renderer)
{
    std::wstring fileN = ToWString(GetRelativePath());
    const wchar_t* fileName = fileN.c_str();
    HRESULT metaDatResult =
        DirectX::GetMetadataFromWICFile(fileName, DirectX::WIC_FLAGS_DEFAULT_SRGB, mMetaData);
    HRESULT imgInitResult = mScratchImage->Initialize(mMetaData);
    HRESULT loadFromFileResult =
        DirectX::LoadFromWICFile(fileName, DirectX::WIC_FLAGS_DEFAULT_SRGB, &mMetaData, *mScratchImage);

    if (FAILED(metaDatResult) ||
        FAILED(imgInitResult) || 
        FAILED(loadFromFileResult))
    {
        LogString("FTTexture::CreateTexture() : Loading texture failed, " + GetRelativePath());
        return false;
    }

    // Create texture.
    D3D11_TEXTURE2D_DESC txtDesc = {};
    txtDesc.Width = mMetaData.width;
    txtDesc.Height = mMetaData.height;
    txtDesc.MipLevels = txtDesc.ArraySize = 1;
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    txtDesc.SampleDesc.Count = 1; // how many multisamples
    txtDesc.SampleDesc.Quality = 0;
    txtDesc.Usage = D3D11_USAGE_IMMUTABLE;
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    int channels = 4;
    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = mScratchImage->GetPixels();
    InitData.SysMemPitch = txtDesc.Width * sizeof(uint8_t) * channels;
    // InitData.SysMemSlicePitch = 0;

    // ID3D11Device* pd3dDevice; // Don't forget to initialize this
    // TODO: You should really consider using a COM smart-pointer like
    // Microsoft::WRL::ComPtr instead

    // ShaderResourceView   specifies the subresources a shader can access during rendering.
    // Examples of shader resources include a constant buffer, a texture buffer, and a texture.

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    if (FAILED(renderer->GetDevice()->CreateTexture2D(&txtDesc, &InitData, texture.GetAddressOf())))
    {
        LogString("FTTexture::CreateTexture() : Create Texture failed, " + GetRelativePath());
        return false;
    }
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    shaderResourceViewDesc.Format = txtDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc.Texture2D.MipLevels = 1;
    if (FAILED(renderer->GetDevice()->CreateShaderResourceView(texture.Get(), &shaderResourceViewDesc,
        mTextureResourceView.GetAddressOf())))
    {
        LogString("FTTexture::CreateTexture() : Create Shader Resource View failed, " + GetRelativePath());
        return false;
    }
    return true;
}

bool FTTexture::ReleaseTexture()
{
    mTextureResourceView.Reset();
    delete mScratchImage;
    mScratchImage = nullptr;
    if (mTextureResourceView.Get() || mScratchImage)
    {
        LogString("FTTexture()::ReleaseTexture() -> Release Texture Failed");
        return false;
    }
    return true;
}
