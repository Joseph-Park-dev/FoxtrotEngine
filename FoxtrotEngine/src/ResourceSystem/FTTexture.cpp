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

    std::string currentPath = "No path has been assigned";
    if (!GetRelativePath().empty())
        currentPath = "Current path : \n" + GetRelativePath();
    ImGui::InputInt("Width", &mTexWidth);
    ImGui::InputInt("Height", &mTexHeight);
}
#endif // FOXTROT_EDITOR