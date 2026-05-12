#pragma once
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
	} // namespace ProcName

	class FTPremade;
	class FTCSV;
	class FTJSON;
	class FTText;

	class ResourceManager :
		public Common::ResourceManagerBase
	{
		SINGLETON(ResourceManager)

	public:
		void LoadDefaultResources() override;

	public:
		FTPremade* GetPremade(const char* key);
		FTCSV*	   GetCSV(const char* key);
		FTJSON*	   GetJSON(const char* key);
		FTText*	   GetText(const char* key);

		Common::ResourcePack<FTPremade>* GetPremades();
		Common::ResourcePack<FTCSV>*	 GetCSVs();
		Common::ResourcePack<FTJSON>*	 GetJSONs();
		Common::ResourcePack<FTText>*	 GetTexts();

	private:
		Common::ResourcePack<FTPremade>* mPremades;
		Common::ResourcePack<FTCSV>*	 mCSVs;
		Common::ResourcePack<FTJSON>*	 mJSONs;
		Common::ResourcePack<FTText>*	 mTexts;
	};

	extern "C"
	{
		CORE_API FTPremade* GetPremade(const char* key);
		CORE_API FTCSV*		GetCSV(const char* key);
		CORE_API FTJSON*	GetJSON(const char* key);
		CORE_API FTText*	GetText(const char* key);

		CORE_API Common::ResourcePack<FTJSON>* GetJSONs();
		CORE_API Common::ResourcePack<FTText>* GetTexts();
	}
} // namespace Core
