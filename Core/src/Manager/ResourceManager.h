#pragma once
#include "Manager/ResourceManagerBase.h"

#include "SingletonMacro.h"
#include "ResourceSystem/ResourcePack.h"

class FTPremade;

namespace Core
{
	class FTPremade;

	class ResourceManager :
		public Common::ResourceManagerBase
	{
		SINGLETON(ResourceManager)

	public:
		void LoadDefaultResources() override;

	public:
		FTPremade* GetPremade(const char* key);

	private:
		Common::ResourcePack<FTPremade>* mPremades;
	};
} // namespace Core
