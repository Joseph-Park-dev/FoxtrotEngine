#include "FTPixelShader.h"

#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"

using namespace Microsoft::WRL;

void FTPixelShader::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
	FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
}

void FTPixelShader::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
	FTResource::LoadProperties(ifs);
}

ComPtr<ID3D11PixelShader>& FTPixelShader::GetShader() { return mShader; }

FTPixelShader::FTPixelShader(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTShader(resDef, renderer)
{
	SetType(ShaderType::PIXEL_SHADER);
	Process(renderer);
}

void FTPixelShader::Process(FoxtrotRenderer* renderer)
{
	LoadMetaData();
	CompileShader(renderer);
	FTResource::Process();
}

void FTPixelShader::CompileShader(FoxtrotRenderer* renderer)
{
	mShader.Reset();

	const wchar_t* fileName = GetRelativePath().WC_Str();
	D3D11Utils::CreatePixelShader(
		renderer->GetDevice(),
		fileName,
		mShader);
	delete[] fileName;
}