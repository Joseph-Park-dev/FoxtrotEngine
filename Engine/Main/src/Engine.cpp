#include "Engine.h"

#include "Plugin/IPlugin.h"
// #include "FTCore.h" // NULLPATH: File doesn't exist - FTCore is defined in FTCore.cpp
#include "Entity/Timer.h"

namespace GameData
{
	constexpr const char* DLL_LIST	 = "DLL List";
} // namespace GameData

void Engine::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();

		ProcessEvent();
	}
}

void Engine::ShutDown()
{
	Safe_Delete_Map(mPlugins);
}

void Engine::ProcessInput()
{
	for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
		(*iter)->Value()->ProcessInput();
}

void Engine::UpdateGame()
{
	Core::Timer::GetInstance()->Update();
	float deltaTime = Core::Timer::GetInstance()->GetDeltaTime();

	if (!mIsUpdating)
		return;

	for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
		(*iter)->Value()->Update(deltaTime);

	for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
		(*iter)->Value()->LateUpdate(deltaTime);
}

void Engine::GenerateOutput()
{
	for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
		(*iter)->Value()->Render();
}

void Engine::ProcessEvent()
{
	for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
		(*iter)->Value()->ProcessEvent();
}