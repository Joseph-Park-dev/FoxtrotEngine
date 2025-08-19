#include "FTJSON.h"

#include <fstream>

#include "Core/FTCore.h"
#include "Static/FTString.h"
#include "FileSystem/FileIOHelper.h"

void FTJSON::Read()
{
	std::ifstream ifs(RelativePath().C_Str());
	mData = nlohmann::json::parse(ifs);
}

nlohmann::json& FTJSON::Data() { return mData; }

void FTJSON::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::JSON::JSON);
	FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::JSON::JSON);
}

void FTJSON::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::JSON::JSON);
	FTResource::LoadProperties(ifs);
}

void FTJSON::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	this->Read();

	this->SetIsProcessed(true);
}