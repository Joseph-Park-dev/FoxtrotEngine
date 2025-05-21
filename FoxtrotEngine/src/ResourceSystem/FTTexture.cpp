// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

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
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"
#include "Renderer/D3D11Utils.h"

#ifdef FOXTROT_EDITOR
#include "EditorLayer.h"
#include <imgui.h>
#endif

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

void FTTexture::Process(FTCore* coreInst)
{
    if (this->GetIsProcessed())
        return;

    FTDS::String path = this->RelativePath().C_Str();
    FTDS::String type = ExtractFileType(path.C_Str());
    FoxtrotRenderer* renderer = coreInst->GetGameRenderer();

    if ((type.Equal(FileTypes::DDS_TEXTURE)))
        DX::ThrowIfFailed(D3D11Utils::CreateCubemapTexture(renderer->GetDevice(), this));
    else
        D3D11Utils::CreateTexture(renderer->GetDevice(), renderer->GetContext(), this);

    if (!this)
        Debug::LogError(__LINE__, __FILE__, "Failed to process Texture.");
    else
        this->SetIsProcessed(true);
}

void FTTexture::SaveProperties(std::ofstream& ofs)
{
    FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTEXTURE);
    FTResource::SaveProperties(ofs);
    FileIOHelper::SaveInt(ofs, ChunkKey::TEXTURE_WIDTH, mTexWidth);
    FileIOHelper::SaveInt(ofs, ChunkKey::TEXTURE_HEIGHT, mTexHeight);
    FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTEXTURE);
}

void FTTexture::LoadProperties(std::ifstream& ifs)
{
    FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTEXTURE);
    FileIOHelper::LoadInt(ifs, mTexHeight);
    FileIOHelper::LoadInt(ifs, mTexWidth);

    FTResource::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void FTTexture::UpdateUI()
{
    ImGui::Text(FileName().C_Str());
    ID3D11ShaderResourceView* viewportTexture = this->mTextureResourceView.Get();
    ImVec2 previewSize = ImVec2(100, 100);
    ImGui::Image((ImTextureID)viewportTexture, previewSize);

    FTDS::String currentPath = "No path has been assigned";
    if (!RelativePath().IsEmpty())
    {
        currentPath.Assign("Current path : \n");
        currentPath.Append(RelativePath().C_Str());
    }

    ImGui::InputInt("Width", &mTexWidth);
    ImGui::InputInt("Height", &mTexHeight);
}
#endif // FOXTROT_EDITOR