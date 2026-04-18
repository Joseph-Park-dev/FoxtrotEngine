#pragma once
#include "Manager/ResourceManagerBase.h"

#include "SingletonMacro.h"
#include "Static/HashMap.h"
#include "FTCore.h"

namespace D3D11
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

#define RES_NAME(type, res) ResourceManager::GetInstance()->GetResName(type::Type, res)
#define GET_RES(type, key) ResourceManager::GetInstance()->GetResource<type>(type::Type, key)
#define GET_CORE_RES reinterpret_cast<Core::ResourceManagerBase*> (ResourceManager::GetInstance()->GetBase()->GetEntities()->At("CoreResourceManager")->Value())
#define CORE_RES_NAME(type, res) GET_CORE_RES->GetResName(type::Type, res)
} // namespace D3D11