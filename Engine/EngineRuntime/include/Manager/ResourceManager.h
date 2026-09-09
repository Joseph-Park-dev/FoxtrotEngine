#pragma once
#include "FTCSV.h"
#include "Manager/ResourceManagerBase.h"

#include "Utility/SingletonMacro.h"
#include "ResourceSystem/ResourcePack.h"
#include "Plugin/CoreExports.h"

namespace Core
{
	namespace ProcName
	{
		constexpr const char* GetJSONs = "GetJSONs";
		constexpr const char* GetTexts = "GetTexts";
		constexpr const char* GetCSVs  = "GetCSVs";

		constexpr const char* GetCSV = "GetCSV";
	} // namespace ProcName

	class FTPremade;
	using FTCSV = GenericData::FTCSV;
} // namespace Core

namespace GenericData
{
	class FTJSON;
	class FTText;
} // namespace GenericData

namespace Core
{

	class ResourceManager
	{
		SINGLETON(ResourceManager)

	public:
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		void LoadDefaultResources();
        /// @brief Releases resources associated with the loaded world.
        void ReleaseWorldResources();

	public:
		/// @brief Returns the premade used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the premade.
		FTPremade*				 GetPremade(const char* key);
		/// @brief Returns the csv used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the csv.
		FTCSV*					 GetCSV(const char* key);
		/// @brief Returns the json used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the json.
		GenericData::FTJSON* GetJSON(const char* key);
		/// @brief Returns the text used by this resource manager.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the text.
		GenericData::FTText* GetText(const char* key);

		/// @brief Returns the premades used by this resource manager.
		/// @return Borrowed access to the premades.
		Common::ResourcePack<FTPremade>*			   GetPremades();
		/// @brief Returns the csvs used by this resource manager.
		/// @return Borrowed access to the csvs.
		Common::ResourcePack<FTCSV>*				   GetCSVs();
		/// @brief Returns the jsons used by this resource manager.
		/// @return Borrowed access to the jsons.
		Common::ResourcePack<GenericData::FTJSON>* GetJSONs();
		/// @brief Returns the texts used by this resource manager.
		/// @return Borrowed access to the texts.
		Common::ResourcePack<GenericData::FTText>* GetTexts();

	private:
		Common::ResourcePack<FTPremade>*				 mPremades;
		Common::ResourcePack<FTCSV>*					 mCSVs;
		Common::ResourcePack<GenericData::FTJSON>* mJSONs;
		Common::ResourcePack<GenericData::FTText>* mTexts;
	};

	extern "C"
	{
		/// @brief Returns the premade used by this service.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the premade.
		CORE_API FTPremade*					 GetPremade(const char* key);
		/// @brief Returns the csv used by this service.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the csv.
		CORE_API FTCSV*						 GetCSV(const char* key);
		/// @brief Returns the json used by this service.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the json.
		CORE_API GenericData::FTJSON* GetJSON(const char* key);
		/// @brief Returns the text used by this service.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the text.
		CORE_API GenericData::FTText* GetText(const char* key);

		/// @brief Returns the jsons used by this service.
		/// @return Borrowed access to the jsons.
		CORE_API Common::ResourcePack<GenericData::FTJSON>* GetJSONs();
		/// @brief Returns the texts used by this service.
		/// @return Borrowed access to the texts.
		CORE_API Common::ResourcePack<GenericData::FTText>* GetTexts();
		/// @brief Returns the csvs used by this service.
		/// @return Borrowed access to the csvs.
		CORE_API Common::ResourcePack<FTCSV>*					 GetCSVs();
	}
} // namespace Core
