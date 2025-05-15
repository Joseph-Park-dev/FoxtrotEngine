#include "FTPixelShader.h"

#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"

void FTPixelShader::CompileShader(FoxtrotRenderer* renderer)
{
	D3D11Utils::CreatePixelShader(
		renderer->GetDevice(),
		ToWString(GetRelativePath()),
		mShader);
}

ComPtr<ID3D11PixelShader>& FTPixelShader::GetShader() { return mShader; }

FTPixelShader::FTPixelShader()
{
	SetType(ShaderType::PIXEL_SHADER);
}

void FTPixelShader::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FT_PIXEL_SHADER);
	FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FT_PIXEL_SHADER);
}

void FTPixelShader::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FT_PIXEL_SHADER);
	FTResource::LoadProperties(ifs);
}

#ifdef FOXTROT_EDITOR
void FTPixelShader::UpdateUI()
{

}
#endif