#include "FTJSON.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "FTCore.h"
#include "Static/FTString.h"
#include "FileSystem/FileIOHelper.h"

namespace Core
{
	Core::ResType		  FTJSON::Type = Core::ResType::JSON;
	const nlohmann::json& FTJSON::Data() const { return mData; }

	// void FTJSON::SaveProperties(std::ofstream& ofs)
	//{
	//	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::JSON::JSON);
	//	FTResource::SaveProperties(ofs);
	//	FileIOHelper::EndDataPackSave(ofs, ChunkKey::JSON::JSON);
	// }
	//
	// void FTJSON::LoadProperties(std::ifstream& ifs)
	//{
	//	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::JSON::JSON);
	//	FTResource::LoadProperties(ifs);
	// }

	FTJSON::FTJSON(Core::FTResourceDef& resDef)
	{
		if (!mData.empty())
			return;

		this->Read(resDef.Path);
	}

	void FTJSON::Read(const Core::FTDS::String& path)
	{
		std::ifstream ifs(path.C_Str());
		mData = nlohmann::json::parse(ifs);
	}
} // namespace Core