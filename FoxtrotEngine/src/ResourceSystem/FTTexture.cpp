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
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"

#ifdef FOXTROT_EDITOR
#include "EditorLayer.h"
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

    std::string currentPath = "No path has been assigned";
    if (!GetRelativePath().empty())
        currentPath = "Current path : \n" + GetRelativePath();
    ImGui::InputInt("Width", &mTexWidth);
    ImGui::InputInt("Height", &mTexHeight);
}
#endif // FOXTROT_EDITOR