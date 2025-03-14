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

#include "Math/FTMath.h"
#include "Managers/ResourceManager.h"

#ifdef FOXTROT_EDITOR
	#include "EditorUtils.h"
#endif

struct MaterialData;
class FTPixelShader;

class FTMaterial : public FTResource
{
private:
	FTPixelShader* mLinkedShader;
	UINT		   mShaderKey;

public:
	virtual void SaveProperties(std::ofstream& ofs, UINT key) = 0;
	virtual UINT LoadProperties(std::ifstream& ifs)			  = 0;

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI() override
	{
		UINT key = mShaderKey;
		FTEditorUtils::DisplayResSelection("Select Shader", ResourceManager::GetInstance()->GetMeshDataMap(), key);
		if (mShaderKey != key)
		{
			mShaderKey	  = key;
			mLinkedShader = ResourceManager::GetInstance()->GetLoadedShader(mShaderKey);
		}

		mLinkedShader->UpdateUI();
	}

#endif // FOXTROT_EDITOR
};

struct MaterialData
{
	FTVector3 Ambient	= FTVector3(0.1f);
	float	  Shininess = 1.0f;
	FTVector3 Diffuse	= FTVector3(0.5f);
	float	  dummy1	= 0.f;
	FTVector3 Specular	= FTVector3(0.5f);
	float	  dummy2	= 0.f;
};

namespace ChunkKey
{
	namespace Material
	{
		constexpr const char* TYPE = "FTMaterial";
		constexpr const char* NAME = "Name";
	} // namespace Material
} // namespace ChunkKey