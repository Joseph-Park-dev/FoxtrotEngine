#pragma once
#include "Manager/ResourceManagerBase.h"

#include "SingletonMacro.h"
#include "Static/HashMap.h"

class FTPremade;

class ResourceManager : public ResourceManagerBase
{
	SINGLETON(ResourceManager)

public:
	void LoadDefaultResources() override;

public:
	void Initialize(FoxtrotRenderer* renderer = nullptr) override;

public:
	// ---------------------------
	// Typed resource lookup APIs
	// ---------------------------
	FTPremade* GetLoadedPremades(FTDS::String&& key);

private:
	FTDS::Array<FTDS::HashMap<FTResource*>*>* mResources;
};
