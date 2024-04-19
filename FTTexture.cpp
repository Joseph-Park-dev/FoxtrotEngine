#include "FTTexture.h"

#include <string>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <d3d11.h>
#include <wrl.h> // ComPtr

#include "CCore.h"
#include "TemplateFunctions.h"

bool FTTexture::CreateTexture(const std::string filename, Microsoft::WRL::ComPtr<ID3D11Device> device)
{
    int width, height, channels;

    unsigned char* img = stbi_load(filename.c_str(), &width, &height, &channels, 0);

    //assert(channels == 4);

    std::vector<uint8_t> image;

    image.resize(width * height * channels);
    memcpy(image.data(), img, image.size() * sizeof(uint8_t));

    // Create texture.
    D3D11_TEXTURE2D_DESC txtDesc = {};
    txtDesc.Width = width;
    txtDesc.Height = height;
    txtDesc.MipLevels = txtDesc.ArraySize = 1;
    txtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    txtDesc.SampleDesc.Count = 1;
    txtDesc.Usage = D3D11_USAGE_IMMUTABLE;
    txtDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    // Fill in the subresource data.
    D3D11_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = image.data();
    InitData.SysMemPitch = txtDesc.Width * sizeof(uint8_t) * channels;
    // InitData.SysMemSlicePitch = 0;

    // ID3D11Device* pd3dDevice; // Don't forget to initialize this
    // TODO: You should really consider using a COM smart-pointer like
    // Microsoft::WRL::ComPtr instead

    // ShaderResourceView   specifies the subresources a shader can access during rendering.
    // Examples of shader resources include a constant buffer, a texture buffer, and a texture.

    if (FAILED(device->CreateTexture2D(&txtDesc, &InitData, mTexture.GetAddressOf())))
    {
        LogString("Create Texture failed : " + filename);
        return false;
    }
    if (FAILED(device->CreateShaderResourceView(mTexture.Get(), nullptr,
        mTextureResourceView.GetAddressOf())))
    {
        LogString("Create Shader Resource View failed : " + filename);
        return false;
    }
    return true;
}
