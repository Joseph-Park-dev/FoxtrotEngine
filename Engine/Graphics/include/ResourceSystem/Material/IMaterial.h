// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "IResource.h"

#include "FTMath.h"

namespace Graphics
{
	/// @brief Base abstract class of all FTMaterials.
	/// When the Material values are modified with GUI, they are applied to the mesh
	/// as the pixel constant buffers.
	class IMaterial :
		public Common::IResource
	{
	public:
		/// @brief Updates pixel constant buffer with material data.
		/// @param context e.g. DeviceContext in D3D11.
		virtual void UpdateBuffer(void* context) = 0;

	public:
		/// @brief Returns the pixel constant buffer with material data applied.
		/// @return e.g. ID3D11Buffer in D3D11.
		virtual void* GetPCBuf() = 0;

	protected:
		/// @brief Creates a pixel constant buffer using the material data.
		/// @param device e.g. ID3D11Device in D3D11.
		virtual void CreatePixelConstBuffer(void* device) = 0;
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
} // namespace Graphics
