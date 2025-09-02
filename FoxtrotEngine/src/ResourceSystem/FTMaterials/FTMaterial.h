// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
#pragma once
#include "ResourceSystem/FTResource.h"

#include "Math/FTMath.h"

struct MaterialData;
class FTPixelShader;

/// @brief Base abstract class of all FTMaterials.
/// When the Material values are modified with GUI, they are applied to the mesh
/// as the pixel constant buffers.
class FTMaterial :
	public FTResource
{
public:
	/// @brief Creates a pixel constant buffer using the material data.
	virtual void CreatePixelConstBuffer(ComPtr<ID3D11Device>& device) = 0;

	/// @brief Updates mPCBuf with the material values.
	virtual void UpdateBuffer(ComPtr<ID3D11DeviceContext>& context) = 0;

	/// @brief See FTResource::SaveProperties();
	virtual void SaveProperties(std::ofstream& ofs) override;

	/// @brief See FTResource::LoadProperties();
	virtual void LoadProperties(std::ifstream& ifs) override;

public:
	/// @brief Gets the pixel constant buffer with the material values applied.
	ComPtr<ID3D11Buffer>& GetPCBuf();

public:
	/// @brief Relative path is used for importing material data.
	FTMaterial(FTResourceDef& resDef, FoxtrotRenderer* renderer);

private:
	/// @brief Pixel constant buffer to which the material values are applied.
	ComPtr<ID3D11Buffer> mPCBuf;

private:
	void Process(FoxtrotRenderer* renderer) override;

#ifdef FOXTROT_EDITOR
public:
	/// @brief GUI update for modifying the material values.
	virtual void UpdateUI() = 0;

#endif
};

namespace ChunkKey
{
	namespace FTMaterial
	{
		constexpr const char* FT_MATERIAL		= "FTMaterial";
		constexpr const char* NAME				= "Name";
		constexpr UINT		  STANDARD_MATERIAL = 1;

	} // namespace FTMaterial
} // namespace ChunkKey