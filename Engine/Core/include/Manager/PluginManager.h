#pragma once
#include "Utility/SingletonMacro.h"

#include "FTDS/Static/HashMap.h"
#include "Plugin/IPlugin.h"
#include "Plugin/PluginKey.h"

namespace Core
{
	class PluginManager
	{
		SINGLETON(PluginManager)

	public:
		// Gameloop functions.
		void ProcessInput();
		void Update(float deltaTime);
		void LateUpdate(float deltaTime);
		void Render();
		void ProcessEvent();

		void ShutDown();

	public:
		Core::IPlugin* RegisterPlugin(const char* pluginName);
		Core::IPlugin* GetPlugin(const char* pluginName);

		Common::FTDS::HashMap<Core::IPlugin*>* GetPlugins();

	public:
		void LoadComponents(Core::IActor* actor);

	private:
		Common::FTDS::HashMap<HMODULE>*		   mModules;
		Common::FTDS::HashMap<Core::IPlugin*>* mPlugins;
	};

	extern "C"
	{
		CORE_API IPlugin* RegisterPlugin(const char* pluginName);
	}
} // namespace Core