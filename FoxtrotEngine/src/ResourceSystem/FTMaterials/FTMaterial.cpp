// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FTMaterial.h"

#include <wrl.h>

#include "Core/FTCore.h"
#include <Renderer/D3D11Utils.h>
#include <Renderer/FoxtrotRenderer.h>

ComPtr<ID3D11Buffer>& FTMaterial::GetPCBuf()
{
	return mPCBuf;
}

FTMaterial::FTMaterial(FTResourceDef& resDef, FoxtrotRenderer* renderer)
	: FTResource(resDef)
{
	Process(renderer);
}

void FTMaterial::Process(FoxtrotRenderer* renderer)
{
	if (this->IsProcessed())
		return;

	std::ifstream ifs(GetRelativePath().C_Str());
	this->LoadProperties(ifs);

	CreatePixelConstBuffer(renderer->GetDevice());
	FTResource::Process();
}