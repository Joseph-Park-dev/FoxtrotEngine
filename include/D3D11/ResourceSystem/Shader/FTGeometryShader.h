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
		Microsoft::WRL::ComPtr<ID3D11GeometryShader>& GetShader();

	public:
		/// @see FTShader::FTShader()
		FTGeometryShader(Core::FTResourceDef& resDef, D3D11Renderer* renderer);

	protected:
		/// @see FTShader::CompileShader()
		void CompileShader(Core::FTResourceDef& resDef, D3D11Renderer* renderer) override;

	private:
		/// @brief The shader should remain compiled after initialization.
		Microsoft::WRL::ComPtr<ID3D11GeometryShader> mShader;
	};
} // namespace D3D11