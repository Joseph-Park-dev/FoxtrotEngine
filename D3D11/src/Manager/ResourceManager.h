#pragma once
#include "Manager/ResourceManagerBase.h"

#include "SingletonMacro.h"
#include "Static/HashMap.h"

namespace D3D11
{
	class ResourceManager : public ResourceManagerBase
	{
		SINGLETON(ResourceManager)

	public:
		void LoadDefaultResources() override;
	};

#define RES_NAME(type, res) ResourceManager::GetInstance()->GetResName(type::Type, res)
#define GET_RES(type, key) ResourceManager::GetInstance()->GetResource<type>(type::Type, key)
#define CORE_RES_NAME(type, res) D3D11::ResourceManager::GetInstance()->GetBase()->GetSingletonInst<ResourceManager>("GetResManager")->GetResName(type::Type, res)
} // namespace D3D11