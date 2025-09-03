#include "FTJSON.h"

#include <fstream>

#include "Core/FTCore.h"
#include "Static/FTString.h"
#include "FileSystem/FileIOHelper.h"

const nlohmann::json& FTJSON::Data() const { return mData; }

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

FTJSON::FTJSON(FTResourceDef& resDef)
	: FTResource(resDef)
{
	Process();
}

void FTJSON::Process()
{
	if (IsProcessed())
		return;

	this->Read();

	FTResource::Process();
}

void FTJSON::Read()
{
	std::ifstream ifs(GetRelativePath().C_Str());
	mData = nlohmann::json::parse(ifs);
}