// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "ResourceSystem/CoreResource.h"

#include <nlohmann/json.hpp>

namespace Core
{
	/// @brief A wrapper class to store JSON data.
	/// Currently using nlohmann JSON library.
	/// @note https://github.com/nlohmann/json
	class FTJSON :
		public Core::CoreResource
	{
		// public:
		//	/// @see FTResource::SaveProperties()
		//	virtual void SaveProperties(std::ofstream& ofs) override;
		//
		//	/// @see FTResource::LoadProperties()
		//	virtual void LoadProperties(std::ifstream& ifs) override;
	public:
		static Core::ResType Type;

	public:
		/// @brief Returns the JSON data as nlohmann::json.
		const nlohmann::json& Data() const;

	public:
		/// @see FTResource::FTResource
		FTJSON(Core::FTResourceDef& resDef);

	private:
		/// @brief .json data.
		nlohmann::json mData;

	private:
		/// @brief Parses the JSON data.
		void Read(const Core::FTDS::String& path);
	};

	namespace ChunkKey
	{
		namespace JSON
		{
			constexpr const char* JSON = "JSON";
		}
	} // namespace ChunkKey
} // namespace Core