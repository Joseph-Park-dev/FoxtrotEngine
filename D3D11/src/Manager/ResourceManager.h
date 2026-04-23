#pragma once
#include "Manager/ResourceManagerBase.h"

#include "SingletonMacro.h"
#include "FTDS/Static/HashMap.h"
// Core Plugin key
#include <../Plugin/PluginKey.h>
// Core ResourceManager
#include <../Manager/ResourceManager.h>

namespace D3D11
{
	class ResourceManager :
		public Core::ResourceManagerBase
	{
		SINGLETON(ResourceManager)
	public:
		void LoadDefaultResources() override;

	public:
		Core::ResourceManager* GetCoreResManager();

	private:
		Core::GET_RES_MANAGER_INST mGetCoreResManagerFunc;
	};

	using GET_RES_MANAGER_INST = D3D11::ResourceManager* (*)();

#define GET_RES(type, key) ResourceManager::GetInstance()->GetResource<type>(type::Type, key)
#define CORE_RES_NAME(type, res) ResourceManager::GetInstance()->GetCoreResManager()->GetResName(type::Type, res)
} // namespace D3D11