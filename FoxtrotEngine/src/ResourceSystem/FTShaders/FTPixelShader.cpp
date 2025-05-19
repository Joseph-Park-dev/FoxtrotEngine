#include "FTPixelShader.h"

#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"

void FTPixelShader::CompileShader(FoxtrotRenderer* renderer)
{
	const wchar_t* fileName = RelativePath().WC_Str();
	D3D11Utils::CreatePixelShader(
		renderer->GetDevice(),
		fileName,
		mShader);
	delete[] fileName;
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