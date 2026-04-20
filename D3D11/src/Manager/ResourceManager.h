#pragma once
#include "Manager/ResourceManagerBase.h"

#include "SingletonMacro.h"
#include "Static/HashMap.h"
// Core Plugin key
#include <../Plugin/PluginKey.h>
// Core ResourceManager
#include <../Manager/ResourceManager.h>

namespace Core
{
	using GET_CORE_RES_MANAGER = Core::ResourceManager* (*)();
}

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

	protected:
		void RegisterMemberFuncs() override;

	private:
		Core::GET_CORE_RES_MANAGER mGetCoreResManagerFunc;
	};

#define RES_NAME(type, res) ResourceManager::GetInstance()->GetResName(type::Type, res)
#define GET_RES(type, key) ResourceManager::GetInstance()->GetResource<type>(type::Type, key)
#define CORE_RES_NAME(type, res) ResourceManager::GetInstance()->GetCoreResManager()->GetResName(type::Type, res)
} // namespace D3D11