#include "Plugin.h"

#include <Windows.h>

#include "Debugging/DebugFuncs.h"
#include "Actor/Actor.h"
#include "Component/Component.h"
#include "Static/FTString.h"

void Plugin::Load()
{
	const wchar_t* path = GetRelativePath().WC_Str();
	mModule				= LoadLibrary(path);
	delete path;
}

void Plugin::Initialize()
{
	for (auto iter = mComponents->Begin(); iter != mComponents->End(); ++iter)
		if (!(*iter)->GetIsInitialized())
			(*iter)->Initialize();
}

void Plugin::Setup()
{
	for (auto iter = mComponents->Begin(); iter != mComponents->End(); ++iter)
		if (!(*iter)->GetIsSetup())
			(*iter)->Setup();
}

void Plugin::ProcessInput(FTInputDevice* inputDevice)
{
	for (auto iter = mComponents->Begin(); iter != mComponents->End(); ++iter)
	{
		if (!(*iter)->GetOwner()->IsActive())
			continue;

		if ((*iter)->GetIsActive())
			(*iter)->ProcessInput(inputDevice);
	}
}

void Plugin::Update(float deltaTime)
{
	for (auto iter = mComponents->Begin(); iter != mComponents->End(); ++iter)
	{
		if (!(*iter)->GetOwner()->IsActive())
			continue;

		if ((*iter)->GetIsActive())
			(*iter)->Update(deltaTime);
	}
}

void Plugin::LateUpdate(float deltaTime)
{
	for (auto iter = mComponents->Begin(); iter != mComponents->End(); ++iter)
	{
		if (!(*iter)->GetOwner()->IsActive())
			continue;

		if ((*iter)->GetIsActive())
			(*iter)->LateUpdate(deltaTime);
	}
}

void Plugin::Render(FoxtrotRenderer* renderer)
{
	for (auto iter = mComponents->Begin(); iter != mComponents->End(); ++iter)
	{
		if (!(*iter)->GetOwner()->IsActive())
			continue;

		if ((*iter)->GetIsActive())
			(*iter)->Render(renderer);
	}
}

void Plugin::Clear()
{
	for (auto iter = mComponents->Begin(); iter != mComponents->End(); ++iter)
	{
		delete (*iter);
		*iter = nullptr;
	}
	mComponents->Clear();
	delete mComponents;
}

Plugin::Plugin(FTResourceDef& resDef, size_t compArrCount)
	: FTResource(resDef)
	, mComponents(DBG_NEW FTDS::DynamicArray<Component*>)
{
	mComponents->Reserve(compArrCount);
	Load();
}

Plugin::~Plugin()
{
	Clear();
	FreeLibrary(mModule);
}

COMP_CONSTRUCTOR Plugin::GetConstructor(const char* procName)
{
	return (COMP_CONSTRUCTOR)(GetProcAddress(mModule, procName));
}