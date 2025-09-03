// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <ResourceSystem/FTShaders/FTShader.h>

#include <wrl.h>

/// @brief A wrapper for HLSL pixel shaders.
class FTPixelShader :
	public FTShader
{
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
	FTPixelShader(FTResourceDef& resDef, FoxtrotRenderer* renderer);

protected:
	/// @see FTShader::CompileShader()
	void CompileShader(FoxtrotRenderer* renderer) override;

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