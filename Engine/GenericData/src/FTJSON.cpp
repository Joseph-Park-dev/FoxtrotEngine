#include "FTJSON.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "FTDS/Static/FTString.h"
#include "FileSystem/FileIOHelper.h"

namespace GenericData
{
	/// @brief Exposes the stored payload for consumers of this resource or container.
	/// @return Borrowed access to the data.
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

	/// @brief Initializes a parsed JSON resource.
	/// @param metaData meta data used by this operation.
	/// @note Initializes the :FTJSON base or delegates to its constructor.
	/// @throws nlohmann::json::parse_error If the resource file contains invalid JSON.
	FTJSON::FTJSON(Common::ResourceData* metaData)
		: Common::FTResource()
	{
		SetFileName(*metaData->FileName);
		SetRelativePath(*metaData->Path);
		if (!mData.empty())
			return;

		this->Read(*mMetaData->Path);
	}

	/// @brief Parses source content into the resource's stored representation.
	/// @param path Filesystem path of the resource or project.
	/// @throws nlohmann::json::parse_error If the input is not valid JSON.
	void FTJSON::Read(const Common::FTDS::String& path)
	{
		std::ifstream ifs(path.C_Str());
		mData = nlohmann::json::parse(ifs);
	}
} // namespace GenericData
