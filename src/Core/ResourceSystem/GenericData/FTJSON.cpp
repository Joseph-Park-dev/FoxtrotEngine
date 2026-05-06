#include "ResourceSystem/GenericData/FTJSON.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "FTDS/Static/FTString.h"
#include "FileSystem/FileIOHelper.h"

namespace Core
{
	const nlohmann::json& FTJSON::Data() const { return mData; }

	// void FTJSON::SaveProperties(std::ofstream& ofs)
	//{
	//	Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::JSON::JSON);
	//	FTResource::SaveProperties(ofs);
	//	Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::JSON::JSON);
	// }
	//
	// void FTJSON::LoadProperties(std::ifstream& ifs)
	//{
	//	Common::FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::JSON::JSON);
	//	FTResource::LoadProperties(ifs);
	// }

	FTJSON::FTJSON(Common::FTResourceDef& resDef)
		: Core::CoreResource(resDef)
	{
		if (!mData.empty())
			return;

		this->Read(resDef.Path);
	}

	void FTJSON::Read(const Common::FTDS::String& path)
	{
		std::ifstream ifs(path.C_Str());
		mData = nlohmann::json::parse(ifs);
	}
} // namespace Core