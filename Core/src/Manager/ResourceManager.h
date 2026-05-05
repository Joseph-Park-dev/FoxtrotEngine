#pragma once
#include "Manager/ResourceManagerBase.h"

#include "SingletonMacro.h"
#include "ResourceSystem/ResourcePack.h"

namespace Core
{
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
	}
} // namespace Core
