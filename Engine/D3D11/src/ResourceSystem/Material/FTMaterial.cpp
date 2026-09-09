// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/Material/FTMaterial.h"

#include <wrl.h>

#include "Utility/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "FileSystem/FileIOHelper.h"

namespace D3D11
{
	using namespace Core;
	using Microsoft::WRL::ComPtr;
	/// @brief Returns the pcbuf used by this ftmaterial.
	/// @return Borrowed access to the pcbuf.
	/// @note Changes through the returned reference affect this object's stored state.
	ComPtr<ID3D11Buffer>& FTMaterial::GetPCBuf()
	{
		return mPCBuf;
	}

	/// @brief Initializes the shared metadata for a rendering material.
	/// @param resDef Resource definition containing the filename and source path.
	/// @note Initializes the :FTMaterial base or delegates to its constructor.
	FTMaterial::FTMaterial(Common::FTResourceDef& resDef)
		: D3D11::D3D11Resource(resDef)
	{
		if (mPCBuf)
			return;

		std::ifstream ifs(resDef.Path);

		if (!ifs.good())
		{
			std::ofstream ofs(resDef.Path);

			FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTMaterial::FT_MATERIAL);
			FTMaterial::SaveProperties(ofs);
			FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTMaterial::FT_MATERIAL);

			FileIOHelper::SaveBufferToFile(ofs);
		}

		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTMaterial::FT_MATERIAL);
		FTMaterial::LoadProperties(ifs);
	}
} // namespace D3D11
