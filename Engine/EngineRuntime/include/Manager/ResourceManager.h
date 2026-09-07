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
		void LoadDefaultResources();
        void ReleaseWorldResources();

	public:
		FTPremade*				 GetPremade(const char* key);
		FTCSV*					 GetCSV(const char* key);
		GenericData::FTJSON* GetJSON(const char* key);
		GenericData::FTText* GetText(const char* key);

		Common::ResourcePack<FTPremade>*			   GetPremades();
		Common::ResourcePack<FTCSV>*				   GetCSVs();
		Common::ResourcePack<GenericData::FTJSON>* GetJSONs();
		Common::ResourcePack<GenericData::FTText>* GetTexts();

	private:
		Common::ResourcePack<FTPremade>*				 mPremades;
		Common::ResourcePack<FTCSV>*					 mCSVs;
		Common::ResourcePack<GenericData::FTJSON>* mJSONs;
		Common::ResourcePack<GenericData::FTText>* mTexts;
	};

	extern "C"
	{
		CORE_API FTPremade*					 GetPremade(const char* key);
		CORE_API FTCSV*						 GetCSV(const char* key);
		CORE_API GenericData::FTJSON* GetJSON(const char* key);
		CORE_API GenericData::FTText* GetText(const char* key);

		CORE_API Common::ResourcePack<GenericData::FTJSON>* GetJSONs();
		CORE_API Common::ResourcePack<GenericData::FTText>* GetTexts();
		CORE_API Common::ResourcePack<FTCSV>*					 GetCSVs();
	}
} // namespace Core
