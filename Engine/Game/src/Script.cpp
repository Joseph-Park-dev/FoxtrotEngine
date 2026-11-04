// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Script.h"

#include <cstring>
#include <fstream>

#include "Actor/IActor.h"
#include "Core/FTCore.h"
#include "Debugging/DebugMemAlloc.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "FTDS/Static/HashMap.h"
#include "Foxtrot/Runtime/RuntimeApi.h"
#include "Plugin/IPlugin.h"

namespace
{
	constexpr size_t MAX_SCRIPT_TYPES = 64;

	struct ScriptType
	{
		const char*			  Name;
		ComponentFactoryEntry Create;
	};

	ScriptType gTypes[MAX_SCRIPT_TYPES]{};
	size_t	   gTypeCount = 0;

	/// @brief Returns the scripts currently enrolled for frame updates.
	Common::FTDS::DynamicArray<Game::Script*>& LiveScripts()
	{
		static Common::FTDS::DynamicArray<Game::Script*> scripts(32);
		return scripts;
	}

	/// @brief Enrolls a script if it is not already enrolled.
	/// @param script Script instance associated with an actor.
	void RegisterLive(Game::Script* script)
	{
		if (!script || LiveScripts().Find(script) != -1)
			return;
		LiveScripts().PushBack(script);
	}

	/// @brief Drops a script from frame updates without deleting it.
	/// @param script Script instance associated with an actor.
	void UnregisterLive(Game::Script* script)
	{
		if (!script)
			return;
		int index = LiveScripts().Find(script);
		if (index < 0)
			return;
		LiveScripts().Erase(static_cast<size_t>(index));
	}

	/// @brief Finds the factory recorded for a script name.
	/// @param name Script name passed to Script::Add.
	/// @return Factory, or nullptr when the name is not registered.
	ComponentFactoryEntry FindFactory(const char* name)
	{
		if (!name)
			return nullptr;
		for (size_t i = 0; i < gTypeCount; ++i)
		{
			if (std::strcmp(gTypes[i].Name, name) == 0)
				return gTypes[i].Create;
		}
		return nullptr;
	}

	/// @brief Runs one lifecycle call on each live script whose actor is active.
	template <typename Func>
	void ForActiveScripts(Func&& call)
	{
		Common::FTDS::DynamicArray<Game::Script*>& scripts = LiveScripts();
		for (size_t i = 0; i < scripts.GetSize(); ++i)
		{
			Game::Script* script = scripts[i];
			if (!script)
				continue;
			Common::IActor* owner = script->GetOwner();
			if (owner && !owner->GetIsActive())
				continue;
			if (!script->GetIsActive())
				continue;
			call(script);
		}
	}

	class ScriptPlugin :
		public Common::IPlugin
	{
	public:
		const char* GetName() override { return "Game"; }

		void RegisterComponent(Common::IComponent* comp) override
		{
			RegisterLive(dynamic_cast<Game::Script*>(comp));
		}

		void ClearRegisteredComponents() override { LiveScripts().Clear(); }

		void SaveProperties(std::ofstream& ofs) override { (void)ofs; }
		void LoadProperties(std::ifstream& ifs) override { (void)ifs; }

		void Initialize() override {}

		void InitializeComponents() override
		{
			LiveScripts().IterateArray([](Game::Script* script) {
				if (!script || script->IsInitialized())
					return;
				script->Initialize();
				script->MarkInitialized();
			});
		}

		void Setup() override
		{
			LiveScripts().IterateArray([](Game::Script* script) {
				if (!script || script->IsSetup())
					return;
				script->Setup();
				script->MarkSetup();
			});
		}

		void ProcessInput() override {}

		void Update(float deltaTime) override
		{
			if (!Core::FTCore::GetInstance()->GetIsUpdating())
				return;

			ForActiveScripts([deltaTime](Game::Script* script) {
				if (!script->IsInitialized())
				{
					script->Initialize();
					script->MarkInitialized();
				}
				if (!script->IsSetup())
				{
					script->Setup();
					script->MarkSetup();
				}
				script->Update(deltaTime);
			});
		}

		void LateUpdate(float deltaTime) override
		{
			if (!Core::FTCore::GetInstance()->GetIsUpdating())
				return;
			ForActiveScripts([deltaTime](Game::Script* script) { script->LateUpdate(deltaTime); });
		}

		void Render() override {}
		void ProcessEvent() override {}
		void ShutDown() override {}

	protected:
		void LoadManagerData(std::ifstream& ifs) override { (void)ifs; }
		void LoadResourceData(std::ifstream& ifs) override { (void)ifs; }
	};

	ScriptPlugin* gPlugin = nullptr;
} // namespace

namespace Game
{
	/// @brief Initializes a script and enrolls it for frame updates.
	/// @param owner Actor that owns this script.
	/// @param name Stable script name.
	/// @param updateOrder Relative order stored with the script.
	Script::Script(Common::IActor* owner, const char* name, int updateOrder)
		: Core::FTBehavior(owner, name, updateOrder)
	{
		RegisterLive(this);
	}

	/// @brief Removes this script from frame updates.
	Script::~Script()
	{
		UnregisterLive(this);
	}

	/// @brief Creates a registered copy on the target actor and copies script state onto it.
	/// @param actor Actor that receives the copy.
	void Script::CloneTo(Common::IActor* actor)
	{
		ComponentFactoryEntry create = FindFactory(GetName());
		if (!create || !actor)
			return;

		Common::IComponent* created = create(actor);
		Script*				clone	= static_cast<Script*>(created);
		if (!clone)
			return;

		CopyBaseStateTo(*clone);
		CopyFieldsTo(*clone);
	}

	/// @brief Copies derived fields onto a clone created by CloneTo.
	/// @param dest Clone that already has this script's base state.
	void Script::CopyFieldsTo(Script& dest)
	{
		(void)dest;
	}

	/// @brief Appends a manually constructed clone to the target actor.
	/// @param actor Actor that will own the clone.
	/// @param clone Script constructed for that actor.
	void Script::AttachClone(Common::IActor* actor, Script* clone)
	{
		if (!actor || !clone || !actor->GetComponents())
			return;
		actor->GetComponents()->PushBack(clone);
	}

	/// @brief Records a script factory under its chunk key.
	/// @param name Script name written to .chunk files.
	/// @param create Factory that constructs the script for an actor.
	/// @return True when the type was recorded.
	bool Script::Register(const char* name, ComponentFactoryEntry create)
	{
		if (!name || !name[0] || !create || MAX_SCRIPT_TYPES <= gTypeCount)
			return false;
		if (FindFactory(name))
			return false;

		gTypes[gTypeCount].Name	  = name;
		gTypes[gTypeCount].Create = create;
		++gTypeCount;
		return true;
	}

	/// @brief Returns the plugin that advances registered scripts.
	/// @return Borrowed plugin pointer owned by the Game module.
	Common::IPlugin* GetScriptPlugin()
	{
		if (!gPlugin)
			gPlugin = DBG_NEW ScriptPlugin();
		return gPlugin;
	}

	/// @brief Registers the script plugin with the runtime.
	/// @param control Runtime control table borrowed from EngineRuntime.
	/// @return Status from the runtime attach call.
	Foxtrot::Status AttachScriptPlugin(Foxtrot::RuntimeControl* control)
	{
		if (!control || !control->Attach)
			return Foxtrot::Status::InvalidArgument;
		return control->Attach("Game", GetScriptPlugin());
	}

	/// @brief Destroys the script plugin owned by the Game module.
	void DestroyScriptPlugin()
	{
		if (!gPlugin)
			return;
		gPlugin->ShutDown();
		delete gPlugin;
		gPlugin = nullptr;
	}
} // namespace Game

/// @brief Publishes script factories exported by Game.dll.
/// @param entries Receives one factory per script registered with Script::Add.
FT_EXPORT void SetComponentFactory(Common::FTDS::HashMap<ComponentFactoryEntry>& entries) noexcept
{
	for (size_t i = 0; i < gTypeCount; ++i)
		entries.Insert(gTypes[i].Name, gTypes[i].Create);
}
