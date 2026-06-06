#pragma once
#include "Utility/SingletonMacro.h"

#include "FTDS/Static/HashMap.h"
#include "FTDS/Static/FTString.h"
#include "Plugin/IPlugin.h"
#include "Plugin/PluginKey.h"

namespace Core
{
	constexpr const char* CREATE_PREFIX = "Create";

	class PluginManager
	{
		SINGLETON(PluginManager)

	public:
		template <typename COMP>
		COMP* RegisterComp(Common::IActor* actor, const char* pluginName)
		{
			Common::FTDS::String procName = CREATE_PREFIX;
			procName.Append(COMP::NAME);
			using construct = COMP* (*)(Common::IActor*);
			return reinterpret_cast<construct>(GetProcAddress(mModules->At(pluginName)->Value(), procName.C_Str()))(actor);
		}

	public:
		// Gameloop functions.
		void ProcessInput();
		void Update(float deltaTime);
		void LateUpdate(float deltaTime);
		void Render();
		void ProcessEvent();

		void ShutDown();

	public:
		Common::IPlugin* RegisterPlugin(const char* pluginName);
		Common::IPlugin* GetPlugin(const char* pluginName);

		Common::FTDS::HashMap<Common::IPlugin*>* GetPlugins();

	public:
		void LoadComponents(Common::IActor* actor);

	private:
		Common::FTDS::HashMap<HMODULE>*		   mModules;
		Common::FTDS::HashMap<Common::IPlugin*>* mPlugins;
	};

	extern "C"
	{
		CORE_API Common::IPlugin* RegisterPlugin(const char* pluginName);
	}
} // namespace Core