// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FTTexture.h"

#include <string>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <d3d11.h>
#include <wrl.h> // ComPtr

#include "TemplateFunctions.h"
#include "D3D11Renderer.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Manager/ResourceManager.h"
#include "D3D11Utils.h"
#include "FTDS/Static/FTString.h"

#ifdef FOXTROT_EDITOR
	#include "EditorLayer.h"
	#include <imgui.h>
#endif

using Microsoft::WRL::ComPtr;

const UINT FTTexture::GetWidth() const
{
	return mWidth;
}

const UINT FTTexture::GetHeight() const
{
	return mHeight;
}

const ComPtr<ID3D11ShaderResourceView>& FTTexture::GetSRV() const
{
	return mSRV;
}

void FTTexture::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTTexture::FT_TEXTURE);
	//FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTTexture::FT_TEXTURE);
}

void FTTexture::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTTexture::FT_TEXTURE);
	//FTResource::LoadProperties(ifs);
}

FTTexture::FTTexture(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: mWidth(0)
	, mHeight(0)
{
	Process(resDef, renderer);
}

FTTexture::~FTTexture()
{
	mSRV.Reset();
	if (mSRV.Get())
		LogString("FTTexture()::ReleaseTexture() -> Release Texture Failed");
}

void FTTexture::Process(FTResourceDef& resDef, FoxtrotRenderer* renderer)
{
	D3D11Renderer* rend = static_cast<D3D11Renderer*>(renderer);
	// Returns early if the resource is processed.
	if (mSRV)
		return;

	// Creates texture
	std::vector<uint8_t> image;
	int					 width	= 0;
	int					 height = 0;
	D3D11Utils::ReadImage(resDef.Path, image, width, height);

	mWidth	= static_cast<UINT>(width);
	mHeight = static_cast<UINT>(height);

	// Copy image data from CPU into staging texture.
	ComPtr<ID3D11Texture2D> stagingTexture =
		D3D11Utils::CreateStagingTexture(rend->GetDevice(), rend->GetContext(), width, height, image);

	// Description for the result texture that will be used.
	D3D11_TEXTURE2D_DESC txtDesc;
	ZeroMemory(&txtDesc, sizeof(txtDesc));
	txtDesc.Width			 = width;
	txtDesc.Height			 = height;
	txtDesc.MipLevels		 = 0; // �Ӹ� ���� �ִ�
	txtDesc.ArraySize		 = 1;
	txtDesc.Format			 = DXGI_FORMAT_R8G8B8A8_UNORM;
	txtDesc.SampleDesc.Count = 1;
	txtDesc.Usage			 = D3D11_USAGE_DEFAULT; // ������¡ �ؽ���κ��� ���� ����
	txtDesc.BindFlags		 = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	txtDesc.MiscFlags		 = D3D11_RESOURCE_MISC_GENERATE_MIPS; // �Ӹ� ���
	txtDesc.CPUAccessFlags	 = 0;

	ComPtr<ID3D11Texture2D> resultTex;

	// Create blank texture (all-black).
	rend->GetDevice()->CreateTexture2D(&txtDesc, nullptr, resultTex.GetAddressOf());

	// Copy staging texture data to the result.
	rend->GetContext()->CopySubresourceRegion(resultTex.Get(), 0, 0, 0, 0, stagingTexture.Get(), 0, nullptr);

	// Create SRV from the resultTex.
	rend->GetDevice()->CreateShaderResourceView(resultTex.Get(), 0, mSRV.GetAddressOf());

	// Create MipMaps.
	rend->GetContext()->GenerateMips(mSRV.Get());
}

#ifdef FOXTROT_EDITOR
void FTTexture::UpdateUI()
{
	// Display texture name.
	ImGui::Text(GetFileName().C_Str());

	// Display texture preview.
	ID3D11ShaderResourceView* viewportTexture = this->mSRV.Get();
	ImVec2					  previewSize	  = ImVec2(100, 100);
	ImGui::Image((ImTextureID)viewportTexture, previewSize);

	// Diplay texture path.
	FTDS::String currentPath = "No path has been assigned";
	if (!GetRelativePath().IsEmpty())
	{
		currentPath.Assign("Current path : \n");
		currentPath.Append(GetRelativePath().C_Str());
	}

	// Update texture size.
	int size[2] = { static_cast<int>(mWidth), static_cast<int>(mHeight) };
	Math::Clamp(size[0], 0, INT_MAX);
	Math::Clamp(size[1], 0, INT_MAX);

	ImGui::InputInt2("Tex size", size);
	mWidth	= static_cast<UINT>(size[0]);
	mHeight = static_cast<UINT>(size[1]);
}
#endif // FOXTROT_EDITOR

extern "C" CORE_API FTResource* CreateResource(FTResourceDef& def, FoxtrotRenderer* rnd)
{
	return new FTTexture(def, rnd);
}