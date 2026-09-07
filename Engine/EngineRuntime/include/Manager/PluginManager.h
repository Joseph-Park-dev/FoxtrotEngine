#pragma once
#include <vector>
#include "Utility/SingletonMacro.h"

#include "FTDS/Static/HashMap.h"
#include "FTDS/Static/FTString.h"
#include "Plugin/IPlugin.h"
#include "Plugin/PluginKey.h"
#include "Plugin/CoreExports.h"

namespace Core
{
	constexpr const char* CREATE_PREFIX = "Create";

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
        void Attach(const char* name, Common::IPlugin* plugin);

	public:
		Common::IPlugin* RegisterPlugin(const char* pluginName);
		Common::IPlugin* GetPlugin(const char* pluginName);

		Common::FTDS::HashMap<Common::IPlugin*>* GetPlugins();

	public:
		void LoadComponents(Common::IActor* actor);

	private:
		std::vector<Common::IPlugin*> mOrder;
		Common::FTDS::HashMap<Common::IPlugin*>* mPlugins;
	};

	extern "C"
	{
		CORE_API Common::IPlugin* RegisterPlugin(const char* pluginName);
	}
} // namespace Core