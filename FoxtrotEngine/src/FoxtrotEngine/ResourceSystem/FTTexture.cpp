#include "FoxtrotEngine/ResourceSystem/FTTexture.h"

#include <string>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <d3d11.h>
#include <wrl.h> // ComPtr

#include "FoxtrotEngine/Core/FTCore.h"
#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "FoxtrotEngine/FileSystem/ChunkLoader.h"

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
    if (mTextureResourceView.Get())
    {
        LogString("FTTexture()::ReleaseTexture() -> Release Texture Failed");
        return false;
    }
    return true;
}

#ifdef _DEBUG
#include "FoxtrotEditor/EditorLayer.h"

void FTTexture::SaveProperties(nlohmann::ordered_json& out)
{
    FileIOHelper::AddScalarValue(out["Width"], mTexWidth);
    FileIOHelper::AddScalarValue(out["Height"], mTexHeight);
}

void FTTexture::UpdateUI(std::string& key)
{
    if (ImGui::BeginListBox(key.c_str(), ImVec2(-FLT_MIN, 200)))
    {
        UpdateKey(key);
        ID3D11ShaderResourceView* viewportTexture = this->mTextureResourceView.Get();
        ImVec2 previewSize = ImVec2(100, 100);
        ImGui::Image((void*)viewportTexture, previewSize);

        //UpdateRelativePath(TEXTURE_FORMAT_SUPPORTED);
        std::string currentPath = "No path has been assigned";
        if (!GetRelativePath().empty())
            currentPath = "Current path : \n" + GetRelativePath();
        ImGui::InputInt("Width", &mTexWidth);
        ImGui::InputInt("Height", &mTexHeight);

        ImGui::EndListBox();
    }
}
#endif // _DEBUG