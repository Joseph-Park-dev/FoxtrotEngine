// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FTMaterial.h"

#include <wrl.h>

#include "FTCore.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/D3D11Renderer.h"
#include "FileSystem/FileIOHelper.h"

namespace D3D11
{
	using namespace Core;
	using Microsoft::WRL::ComPtr;
	ComPtr<ID3D11Buffer>& FTMaterial::GetPCBuf()
	{
		return mPCBuf;
	}

	FTMaterial::FTMaterial(Core::FTResourceDef& resDef, D3D11Renderer* renderer)
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

		CreatePixelConstBuffer(renderer->GetDevice());
	}
} // namespace D3D11