#include "RimMaterial.h"

#include <directxtk/SimpleMath.h>

#include "Debugging/DebugMemAlloc.h"
#include "FileSystem/FileIOHelper.h"
#include "Core/TemplateFunctions.h"
#include "Renderer/D3D11Utils.h"

#ifdef FOXTROT_EDITOR
	#include "DirectoryHelper.h"
	#include "CommandHistory.h"
#endif // FOXTROT_EDITOR

void RimMaterial::CreatePixelConstBuffer(ComPtr<ID3D11Device>& device)
{
	D3D11Utils::CreateConstantBuffer(device, *mData, GetPCBuf());
}

void RimMaterial::UpdateBuffer(ComPtr<ID3D11DeviceContext>& context)
{
	D3D11Utils::UpdateBuffer(context, *mData, GetPCBuf());
}


void RimMaterial::LoadProperties(std::ifstream& ifs)
{
	std::ifstream ifs(RelativePath().C_Str());

	if (ifs)
	{
		FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::RIM_MAT);

		bool boolVal = false;
		FileIOHelper::LoadBool(ifs, boolVal);
		mData->mUseSmoothstep = boolVal;

		FileIOHelper::LoadFloat(ifs, mData->mRimStrength);
		FileIOHelper::LoadFloat(ifs, mData->mRimPower);

		FTVector3 vec3 = FTVector3::Zero;

		FileIOHelper::LoadVector3(ifs, vec3);
		mData->mRimColor = vec3.GetDXVec3();
	}
	else
		Debug::LogError(__LINE__, __FILE__, "Failed to load material from file");
}

RimMaterial::RimMaterial()
	: FTMaterial()
	, mData(DBG_NEW RimData)
{
	FTDS::String name = FTDS::String(ChunkKey::RIM_MAT) + FileTypes::MATERIAL;
#ifdef FOXTROT_EDITOR
	FTDS::String path = PATH_PROJECT +"\\Assets\\Materials\\" + name;
#else
	FTDS::String path = FTDS::String(".\\Assets\\Materials\\") + name;
#endif // FOXTROT_EDITOR

	SetFileName(name);
	SetRelativePath(path);
}

RimMaterial::~RimMaterial()
{
	delete mData;
}

void RimMaterial::SaveProperties(std::ofstream& ofs)
{
	std::ofstream ofs(RelativePath().C_Str());

	if (ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::RIM_MAT);

		FileIOHelper::SaveVector3(ofs, ChunkKey::Rim::COLOR, mData->mRimColor);
		FileIOHelper::SaveFloat(ofs, ChunkKey::Rim::POWER, mData->mRimPower);
		FileIOHelper::SaveFloat(ofs, ChunkKey::Rim::STRENGTH, mData->mRimStrength);
		FileIOHelper::SaveBool(ofs, ChunkKey::Rim::USE_SMOOTH_STEP, mData->mUseSmoothstep);

		FileIOHelper::EndDataPackSave(ofs, ChunkKey::RIM_MAT);
		FileIOHelper::SaveBufferToFile(ofs);

		printf("Material %s created to %s\n", FileName().C_Str(), RelativePath().C_Str());
	}
	else
		Debug::LogError(__LINE__, __FILE__, "Failed to save material to file");
}

#ifdef FOXTROT_EDITOR
void RimMaterial::UpdateUI()
{
	bool val = (bool)mData->mUseSmoothstep;
	CommandHistory::GetInstance()->UpdateVector3Value("Rim Color", mData->mRimColor);
	CommandHistory::GetInstance()->UpdateFloatValue("Rim Power", mData->mRimPower);
	CommandHistory::GetInstance()->UpdateFloatValue("Rim Strength", mData->mRimStrength);
	CommandHistory::GetInstance()->UpdateBoolValue("Use Smooth Step", val);
	mData->mUseSmoothstep = val;
}
#endif