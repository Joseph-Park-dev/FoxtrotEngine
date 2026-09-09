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
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @see FTResource::SaveProperties()
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @see FTResource::LoadProperties()
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;

	public:
		/// @brief Returns compiled HLSL pixel shader object.
		/// @return Borrowed access to the shader.
		/// @note Changes through the returned reference affect this object's stored state.
		Microsoft::WRL::ComPtr<ID3D11PixelShader>& GetShader();

	public:
		/// @brief Initializes a pixel-shader resource from its definition.
		/// @see FTShader::FTShader()
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
		FTPixelShader(Common::FTResourceDef& resDef, void* renderer);

	protected:
		/// @brief Compiles shader source into bytecode using the shader's configured entry point and profile.
		/// @see FTShader::CompileShader()
		/// @param resDef Resource definition containing the filename and source path.
		/// @param renderer Renderer providing the graphics device and current render state.
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
