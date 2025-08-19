// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base class of all FTMaterials.
/// After its value modified with UI, it returns the core data
/// through the AssignData() member function
/// (the argument reference must be defined as well).
/// </summary>

#pragma once
#include "ResourceSystem/FTResource.h"

#include <d3d11.h>
#include <wrl.h>

#include "Math/FTMath.h"

struct MaterialData;
class FTPixelShader;

using namespace Microsoft::WRL;

class FTMaterial : public FTResource
{
public:
	virtual void CreatePixelConstBuffer(ComPtr<ID3D11Device>& device) = 0;
	virtual void UpdateBuffer(ComPtr<ID3D11DeviceContext>& context)	  = 0;

	virtual void SaveToFile()	= 0;
	virtual void LoadFromFile() = 0;

	void Process(FTCore* coreInst) override;

public:
	ComPtr<ID3D11Buffer>& GetPCBuf();

private:
	ComPtr<ID3D11Buffer> mPCBuf;

#ifdef FOXTROT_EDITOR
public:
	virtual void UpdateUI() = 0;
#endif
};

namespace ChunkKey
{
	constexpr const char* FTMATERIAL = "FTMaterial";
	namespace Material
	{
		constexpr const char* NAME				= "Name";
		constexpr UINT		  STANDARD_MATERIAL = 1;
	} // namespace Material
} // namespace ChunkKey