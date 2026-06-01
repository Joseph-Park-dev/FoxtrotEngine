#include "ResourceSystem/Shader/FTGeometryShader.h"

#include "Utility/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "Manager/ResourceManager.h"

namespace D3D11
{
	using namespace Common;
	ResType FTGeometryShader::Type = ResType::GEOMETRY_SHADER;

	Microsoft::WRL::ComPtr<ID3D11GeometryShader>& FTGeometryShader::GetShader()
	{
		return mShader;
	}

	FTGeometryShader::FTGeometryShader(Common::FTResourceDef& resDef, void* renderer)
		: D3D11::FTShader(resDef)
	{
		SetType(ShaderType::GEOMETRY_SHADER);
		D3D11::D3D11Renderer* rend = reinterpret_cast<D3D11::D3D11Renderer*>(renderer);
		CompileShader(resDef, rend);
	}

	void FTGeometryShader::CompileShader(Common::FTResourceDef& resDef, D3D11Renderer* renderer)
	{
		mShader.Reset();

		Common::FTDS::String path		 = resDef.Path;
		const wchar_t*	   wcharPath = path.WC_Str();
		D3D11Utils::CreateGeometryShader(
			renderer->GetDevice(),
			wcharPath,
			mShader);
		delete[] wcharPath;
	}
} // namespace D3D11