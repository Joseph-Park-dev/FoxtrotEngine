#pragma once
#include "FTShader.h"

class D3D11Renderer;

namespace D3D11
{
	class FTGeometryShader :
		public FTShader
	{
	public:
		/// @brief Returns compiled HLSL pixel shader object.
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GetShader();

	public:
		/// @see FTShader::FTShader()
		FTGeometryShader(FTResourceDef& resDef, D3D11Renderer* renderer);

	protected:
		/// @see FTShader::CompileShader()
		void CompileShader(FTResourceDef& resDef, D3D11Renderer* renderer) override;

	private:
		/// @brief The shader should remain compiled after initialization.
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> mShader;
	};
} // namespace D3D11