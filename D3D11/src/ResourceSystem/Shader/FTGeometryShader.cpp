#include "FTGeometryShader.h"

#include "Renderer/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "Manager/ResourceManager.h"

namespace D3D11
{
	ResType FTGeometryShader::Type = ResType::PIXEL_SHADER;

	Microsoft::WRL::ComPtr<ID3D11GeometryShader>& FTGeometryShader::GetShader()
	{
		return mShader;
	}

	FTGeometryShader::FTGeometryShader(FTResourceDef& resDef, D3D11Renderer* renderer)
		: FTShader(resDef, renderer)
	{
		SetType(ShaderType::GEOMETRY_SHADER);
		CompileShader(resDef, renderer);
	}

	void FTGeometryShader::CompileShader(FTResourceDef& resDef, D3D11Renderer* renderer)
	{
		mShader.Reset();

		FTDS::String   path		 = resDef.Path;
		const wchar_t* wcharPath = path.WC_Str();
		D3D11Utils::CreateGeometryShader(
			renderer->GetDevice(),
			wcharPath,
			mShader);
		delete[] wcharPath;
	}
} // namespace D3D11