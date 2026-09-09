#pragma once
#include "FTShader.h"

#include <wrl.h>
#include <d3d11.h>

namespace D3D11
{
	class FTGeometryShader :
		public FTShader
	{
	public:
		static D3D11::ResType Type;

	public:
		/// @brief Returns compiled HLSL pixel shader object.
		/// @return Borrowed access to the shader.
		/// @note Changes through the returned reference affect this object's stored state.
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GetShader();

	public:
		/// @brief Initializes a geometry-shader resource from its definition.
		/// @see FTShader::FTShader()
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		FTGeometryShader(Common::FTResourceDef& resDef, void* renderer);

	protected:
		/// @brief Compiles shader source into bytecode using the shader's configured entry point and profile.
		/// @see FTShader::CompileShader()
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		void CompileShader(Common::FTResourceDef& resDef, D3D11Renderer* renderer) override;

	private:
		/// @brief The shader should remain compiled after initialization.
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> mShader;
	};
} // namespace D3D11
