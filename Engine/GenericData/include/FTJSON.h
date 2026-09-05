// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "IResource.h"

#include <nlohmann/json.hpp>

namespace GenericData
{
	/// @brief A wrapper class to store JSON data.
	/// Currently using nlohmann JSON library.
	/// @note https://github.com/nlohmann/json
	class FTJSON :
		public Common::IResource
	{
		// public:
		//	/// @see FTResource::SaveProperties()
		//	virtual void SaveProperties(std::ofstream& ofs) override;
		//
		//	/// @see FTResource::LoadProperties()
		//	virtual void LoadProperties(std::ifstream& ifs) override;
	public:
		/// @brief Returns the JSON data as nlohmann::json.
		virtual const nlohmann::json& Data() const;

	public:
		/// @see FTResource::FTResource
		FTJSON(Common::ResourceData* resDef);

	private:
		Common::ResourceData* mMetaData;
		/// @brief .json data.
		nlohmann::json mData;

	private:
		/// @brief Parses the JSON data.
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