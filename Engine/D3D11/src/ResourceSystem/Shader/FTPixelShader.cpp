#include "ResourceSystem/Shader/FTPixelShader.h"

#include "Renderer/D3D11Renderer.h"
#include "Utility/D3D11Utils.h"
#include "FileSystem/FileIOHelper.h"
#include "Manager/ResourceManager.h"

using namespace Microsoft::WRL;

namespace D3D11
{
	using namespace Common;
	ResType FTPixelShader::Type = ResType::PIXEL_SHADER;

	void FTPixelShader::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
		D3D11Resource::SaveProperties(ofs);
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
	}

	void FTPixelShader::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
		D3D11Resource::LoadProperties(ifs);
	}

	ComPtr<ID3D11PixelShader>& FTPixelShader::GetShader() { return mShader; }

	FTPixelShader::FTPixelShader(Common::FTResourceDef& resDef, D3D11Renderer* renderer)
		: FTShader(resDef)
	{
		SetType(ShaderType::PIXEL_SHADER);
		LoadMetaData(resDef);
		CompileShader(resDef, renderer);
	}

	void FTPixelShader::CompileShader(Common::FTResourceDef& resDef, D3D11Renderer* renderer)
	{
		mShader.Reset();

		Common::FTDS::String   path		 = resDef.Path;
		const wchar_t* wcharPath = path.WC_Str();
		D3D11Utils::CreatePixelShader(
			renderer->GetDevice(),
			wcharPath,
			mShader);
		delete[] wcharPath;
	}
} // namespace D3D11