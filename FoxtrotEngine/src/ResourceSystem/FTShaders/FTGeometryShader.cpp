#include "FTGeometryShader.h"

#include "Renderer/D3D11Utils.h"
#include "Renderer/FoxtrotRenderer.h"

Microsoft::WRL::ComPtr<ID3D11GeometryShader>& FTGeometryShader::GetShader()
{
	return mShader;
}

FTGeometryShader::FTGeometryShader(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTShader(resDef, renderer)
{
	SetType(ShaderType::GEOMETRY_SHADER);
	Process(renderer);
}

void FTGeometryShader::Process(FoxtrotRenderer* renderer)
{
	CompileShader(renderer);
	FTResource::Process();
}

void FTGeometryShader::CompileShader(FoxtrotRenderer* renderer)
{
	mShader.Reset();

	const wchar_t* fileName = GetRelativePath().WC_Str();
	D3D11Utils::CreateGeometryShader(
		renderer->GetDevice(),
		fileName,
		mShader);
	delete[] fileName;
}