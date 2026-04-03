#pragma once
#include "Manager/ResourceManagerBase.h"

#include "SingletonMacro.h"
#include "Static/HashMap.h"

class FTPremade;

namespace Core
{
	class ResourceManager : public ResourceManagerBase
	{
		SINGLETON(ResourceManager)

	public:
		void LoadDefaultResources() override;
	};

#define RES_NAME(idx, res) ResourceManager::GetInstance()->GetResName(idx, res)
#define GET_RES(type, key) ResourceManager::GetInstance()->GetResource<type>(type::Type, key)
} // namespace Core