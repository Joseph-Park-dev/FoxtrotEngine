// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/FTResource.h"

#include <nlohmann/json.hpp>

namespace GenericData
{
	/// @brief A wrapper class to store JSON data.
	/// Currently using nlohmann JSON library.
	/// @note https://github.com/nlohmann/json
	class FTJSON :
		public Common::FTResource
	{
		// public:
		//	/// @see FTResource::SaveProperties()
		//	virtual void SaveProperties(std::ofstream& ofs) override;
		//
		//	/// @see FTResource::LoadProperties()
		//	virtual void LoadProperties(std::ifstream& ifs) override;
	public:
		/// @brief Returns the JSON data as nlohmann::json.
		/// @return Borrowed access to the data.
		virtual const nlohmann::json& Data() const;

	public:
		/// @brief Initializes a parsed JSON resource.
		/// @see FTResource::FTResource
		/// @param resDef Resource definition containing the filename and source path.
		/// @throws nlohmann::json::parse_error If the resource file contains invalid JSON.
		FTJSON(Common::ResourceData* resDef);

	private:

		/// @brief .json data.
		nlohmann::json mData;

	private:
		/// @brief Parses the JSON data.
		/// @param path Filesystem path of the resource or project.
		/// @throws nlohmann::json::parse_error If the input is not valid JSON.
		void Read(const Common::FTDS::String& path);
	};

	namespace ChunkKey
	{
		namespace JSON
		{
			constexpr const char* JSON = "JSON";
		}
	} // namespace ChunkKey
} // namespace GenericData
