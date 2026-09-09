#include "ResourceSystem/Shader/FTGeometryShader.h"

#include "Utility/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "Manager/ResourceManager.h"

namespace D3D11
{
	using namespace Common;
	ResType FTGeometryShader::Type = ResType::GEOMETRY_SHADER;

	/// @brief Returns the shader used by this ftgeometry shader.
	/// @return Borrowed access to the shader.
	/// @note Changes through the returned reference affect this object's stored state.
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>& FTGeometryShader::GetShader()
	{
		return mShader;
	}

	/// @brief Initializes a geometry-shader resource from its definition.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @note Initializes the :FTGeometryShader base or delegates to its constructor.
	FTGeometryShader::FTGeometryShader(Common::FTResourceDef& resDef, void* renderer)
		: D3D11::FTShader(resDef)
	{
		SetType(ShaderType::GEOMETRY_SHADER);
		D3D11::D3D11Renderer* rend = reinterpret_cast<D3D11::D3D11Renderer*>(renderer);
		CompileShader(resDef, rend);
	}

	/// @brief Compiles shader source into bytecode using the shader's configured entry point and profile.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param renderer Renderer providing the graphics device and current render state.
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
