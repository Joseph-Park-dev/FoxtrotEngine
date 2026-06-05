// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/Shader/FTShader.h"

#include <wrl.h>
#include <d3d11.h>

namespace D3D11
{
	/// @brief A wrapper for HLSL pixel shaders.
	class FTPixelShader :
		public FTShader
	{
	public:
		static D3D11::ResType Type;

	public:
		/// @see FTResource::SaveProperties()
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @see FTResource::LoadProperties()
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		/// @brief Returns compiled HLSL pixel shader object.
		Microsoft::WRL::ComPtr<ID3D11PixelShader>& GetShader();

	public:
		/// @see FTShader::FTShader()
		FTPixelShader(Common::FTResourceDef& resDef, D3D11Renderer* renderer);

	protected:
		/// @see FTShader::CompileShader()
		void CompileShader(Common::FTResourceDef& resDef, D3D11Renderer* renderer) override;

	private:
		/// @brief The shader should remain compiled after initialization.
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mShader;
	};

	namespace ChunkKey
	{
		namespace FTPixelShader
		{
			constexpr const char* FT_PIXEL_SHADER = "FTPixelShader";
		}
	} // namespace ChunkKey
} // namespace D3D11