#pragma once
#include "Manager/ResourceManagerBase.h"

#include "SingletonMacro.h"
#include "Static/HashMap.h"
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

	protected:
		void RegisterMemberFuncs() override;
	};

	// extern "C" CORE_API ResourceManager* GetResManager();

#define RES_NAME(type, res) ResourceManager::GetInstance()->GetResName(type::Type, res)
#define GET_RES(type, key) ResourceManager::GetInstance()->GetResource<type>(type::Type, key)
} // namespace Core