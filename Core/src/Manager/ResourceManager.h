#pragma once
#include "SingletonMacro.h"

#include "FTDS/Static/HashMap.h"
#include "Plugin/CoreExports.h"
#include "FTCore.h"

class FTPremade;

namespace Core
{
	class ResourceManager
	{
		SINGLETON(ResourceManager)

	public:
		void LoadDefaultResources() override;
	};

	using GET_RES_MANAGER_INST = Core::ResourceManager* (*)();

#define RES_NAME(type, res) ResourceManager::GetInstance()->GetResName(type::Type, res)
#define GET_RES(type, key) ResourceManager::GetInstance()->GetResource<type>(type::Type, key)

	// Exported factory — C linkage, stable ABI
	extern "C" CORE_API ResourceManager* GetCoreResourceManager();
} // namespace Core
