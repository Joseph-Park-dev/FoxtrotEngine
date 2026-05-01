#pragma once
#include "Manager/ResourceManagerBase.h"

#include "SingletonMacro.h"
#include "FTDS/Static/HashMap.h"
#include "Plugin/CoreExports.h"
#include "FTCore.h"

class FTPremade;

namespace Core
{
	class ResourceManager :
		public Core::ResourceManagerBase
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

#ifdef FOXTROT_EDITOR
namespace Editor
{
	using GET_CORE_RES_MANAGER = Core::ResourceManager* (*)();
	constexpr const char* GET_CORE_RES_MANAGER_FUNC = "GetCoreResourceManager";
	extern CORE_API GET_CORE_RES_MANAGER gGetCoreResManagerFunc;
	#define CORE_RES_MANAGER() gGetCoreResManagerFunc();
}
#endif // FOXTROT_EDITOR
