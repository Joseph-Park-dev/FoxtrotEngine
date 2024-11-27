#include "ResourceSystem/FTTexture.h"

#include <string>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <d3d11.h>
#include <wrl.h> // ComPtr

#include "Core/FTCore.h"
#include "Core/TemplateFunctions.h"
#include "Renderer/FoxtrotRenderer.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"


#ifdef FOXTROT_EDITOR
#include "EditorLayer.h"
#endif

bool FTTexture::CreateTexture(
    FoxtrotRenderer* renderer, 
    const std::string filename
)
{
    int width, height, channels;

    unsigned char* img =
        stbi_load(filename.c_str(), &width, &height, &channels, 0);

    //assert(channels == 4);

    mTexWidth = width;
    mTexHeight = height;

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

    // ShaderResourceView specifies the subresources a shader can access during rendering.
    // Examples of shader resources include a constant buffer, a texture buffer, and a texture.
    DX::ThrowIfFailed(
        renderer->GetDevice()->CreateTexture2D(&txtDesc, &InitData, mTexture.GetAddressOf())
    );
    DX::ThrowIfFailed(
        renderer->GetDevice()->CreateShaderResourceView(mTexture.Get(), nullptr,
            mTextureResourceView.GetAddressOf())
    );
    return mTexture.Get() != nullptr;
}

bool FTTexture::ReleaseTexture()
{
    mTextureResourceView.Reset();
    mTexture.Reset();
    mSamplerState.Reset();
    if (mTextureResourceView.Get() || mTexture.Get() || mSamplerState.Get())
    {
        LogString("FTTexture()::ReleaseTexture() -> Release Texture Failed");
        return false;
    }
    return true;
}

void FTTexture::SaveProperties(std::ofstream& ofs, UINT key)
{
    FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::FTTEXTURE);
    FTResource::SaveProperties(ofs, key);
    FileIOHelper::SaveInt(ofs, ChunkKeys::TEXTURE_WIDTH, mTexWidth);
    FileIOHelper::SaveInt(ofs, ChunkKeys::TEXTURE_HEIGHT, mTexHeight);
    FileIOHelper::EndDataPackSave(ofs, ChunkKeys::FTTEXTURE);
}

UINT FTTexture::LoadProperties(std::ifstream& ifs)
{
    FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FTTEXTURE);
    FileIOHelper::LoadInt(ifs, mTexHeight);
    FileIOHelper::LoadInt(ifs, mTexWidth);
    UINT key = FTResource::LoadProperties(ifs);
    return key;
}

#ifdef FOXTROT_EDITOR
void FTTexture::UpdateUI()
{
    ImGui::Text(GetFileName().c_str());
    ID3D11ShaderResourceView* viewportTexture = this->mTextureResourceView.Get();
    ImVec2 previewSize = ImVec2(100, 100);
    ImGui::Image((void*)viewportTexture, previewSize);

    //UpdateRelativePath(TEXTURE_FORMAT_SUPPORTED);
    std::string currentPath = "No path has been assigned";
    if (!GetRelativePath().empty())
        currentPath = "Current path : \n" + GetRelativePath();
    ImGui::InputInt("Width", &mTexWidth);
    ImGui::InputInt("Height", &mTexHeight);
}
#endif // FOXTROT_EDITOR