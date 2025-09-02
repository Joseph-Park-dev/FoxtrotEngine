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
	/// @brief Updates pixel constant buffer with material data.
	virtual void UpdateBuffer(ComPtr<ID3D11DeviceContext>& context) = 0;

public:
	/// @brief Gets the pixel constant buffer with material data applied.
	ComPtr<ID3D11Buffer>& GetPCBuf();

public:
	/// @brief Relative path is used for importing material data.
	FTMaterial(FTResourceDef& resDef, FoxtrotRenderer* renderer);

protected:
	/// @brief Creates a pixel constant buffer using the material data.
	virtual void CreatePixelConstBuffer(ComPtr<ID3D11Device>& device) = 0;

private:
	/// @brief Pixel constant buffer to which material data are applied.
	ComPtr<ID3D11Buffer> mPCBuf;

private:
	/// @see FTResource::Process()
	void Process(FoxtrotRenderer* renderer) override;

#ifdef FOXTROT_EDITOR
public:
	/// @brief GUI update function for modifying the material data.
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