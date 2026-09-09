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

	/// @brief Serializes this object's persistent properties to a .chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	/// @note Writes to the supplied stream at its current position.
	void FTPixelShader::SaveProperties(std::ofstream& ofs)
	{
		Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
		D3D11Resource::SaveProperties(ofs);
		Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
	}

	/// @brief Restores this object's persistent properties from a .chunk stream.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @note Advances the stream position and updates the destination state.
	void FTPixelShader::LoadProperties(std::ifstream& ifs)
	{
		Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPixelShader::FT_PIXEL_SHADER);
		D3D11Resource::LoadProperties(ifs);
	}

	/// @brief Returns the shader used by this ftpixel shader.
	/// @return Borrowed access to the shader.
	/// @note Changes through the returned reference affect this object's stored state.
	ComPtr<ID3D11PixelShader>& FTPixelShader::GetShader() { return mShader; }

	/// @brief Initializes a pixel-shader resource from its definition.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @note Initializes the :FTPixelShader base or delegates to its constructor.
	FTPixelShader::FTPixelShader(Common::FTResourceDef& resDef, void* renderer)
		: FTShader(resDef)
	{
		SetType(ShaderType::PIXEL_SHADER);
		LoadMetaData(resDef);

		D3D11::D3D11Renderer* rend = reinterpret_cast<D3D11::D3D11Renderer*>(renderer);
		CompileShader(resDef, rend);
	}

	/// @brief Compiles shader source into bytecode using the shader's configured entry point and profile.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param renderer Renderer providing the graphics device and current render state.
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
