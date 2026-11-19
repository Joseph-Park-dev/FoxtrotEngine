// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "pch.h"
#include "Plugin/Sound.h"

#include <cstring>
#include <fstream>

#include "Actor/ActorData.h"
#include "Actor/IActor.h"
#include "Component/AudioPlayer.h"
#include "Core/FTCore.h"
#include "Debugging/DebugFuncs.h"
#include "Debugging/DebugMemAlloc.h"
#include "Factory/IComponentFactory.h"
#include "Factory/IFTResourceFactory.h"
#include "Foxtrot/Core/ModuleApi.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "FTDS/Static/HashMap.h"
#include "Manager/ResourceManager.h"
#include "Plugin/IPlugin.h"
#include "Plugin/SoundExports.h"
#include "ResourceSystem/FTSound.h"

#include "soloud.h"

#ifdef FOXTROT_EDITOR
	#include "FileSystem/DLLPath.h"
	#include "Plugin/GetFunc.h"
	#include "Plugin/ICommandHistory.h"
	#include "Plugin/IEditorResourceService.h"
	#include "Plugin/PluginKey.h"
	#include <imgui.h>
#endif

#ifdef FOXTROT_EDITOR
namespace Editor
{
	static ICommandHistory* sCommandHistory = nullptr;

	void AcquireCommandHistory()
	{
		if (sCommandHistory != nullptr)
			return;

		sCommandHistory = Core::GetFunc<GET_CMD_HISTORY>(
			Common::DLLPath::EDITOR,
			Core::ProcNames::CMDHistory)();
	}

	ICommandHistory* GetCommandHistory()
	{
		if (!sCommandHistory)
			AcquireCommandHistory();
		return sCommandHistory;
	}
} // namespace Editor
#endif

namespace Sound
{
#ifdef FOXTROT_EDITOR
	class SoundEditorResourceService;
#endif

	class SoundPlugin : public Common::IPlugin
	{
	public:
		const char* GetName() override;
		void*		QueryInterface(const char* name) noexcept override;
		void		RegisterComponent(Common::IComponent* comp) override;
		void		UnregisterComponent(Common::IComponent* comp);
		void		ClearRegisteredComponents() override;
		void		SaveProperties(std::ofstream& ofs) override;
		void		LoadProperties(std::ifstream& ifs) override;
		void		Initialize() override;
		void		InitializeComponents() override;
		void		Setup() override;
		void		ProcessInput() override;
		void		Update(float deltaTime) override;
		void		LateUpdate(float deltaTime) override;
		void		Render() override;
		void		ProcessEvent() override;
		void		ShutDown() override;

		explicit SoundPlugin(const char* name);
		~SoundPlugin() override;

		static SoundPlugin* GetInstance();
		/// @brief Returns the live SoLoud engine, or nullptr before playback starts.
		SoLoud::Soloud* Engine() const;

	protected:
		void LoadManagerData(std::ifstream& ifs) override;
		void LoadResourceData(std::ifstream& ifs) override;

	private:
		void StartEngine();
		void StopEngine();

		Common::FTDS::DynamicArray<Common::IComponent*>* mRegisteredComps;
		SoLoud::Soloud*									 mEngine;
		bool											 mEngineReady;
#ifdef FOXTROT_EDITOR
		SoundEditorResourceService* mEditorResources = nullptr;
#endif
		static SoundPlugin* mInstance;
	};

	SoundPlugin* SoundPlugin::mInstance = nullptr;

#ifdef FOXTROT_EDITOR
	class SoundEditorResourceService final : public Core::IEditorResourceService
	{
	public:
		bool LoadDefaultResources(const char*, Graphics::IRenderer*) noexcept override
		{
			return true;
		}

		bool LoadAllResourcesInAsset(const char* assetDirectory, Graphics::IRenderer*) noexcept override
		{
			if (!assetDirectory || !*assetDirectory)
				return false;
			Common::FTDS::String directory(assetDirectory);
			ResourceManager::GetInstance()->LoadAllResourcesInAsset(&directory);
			return true;
		}

		void DisplayLoadedResources() override
		{
			ResourceManager::GetInstance()->EditorUIUpdate();
		}

		void SaveReferencedResources(std::ofstream& ofs) override
		{
			if (Common::ResourcePack<FTSound>* sounds = ResourceManager::GetInstance()->GetSounds())
				sounds->SaveReferencedResourcesToChunk(ofs);
		}

		void LoadReferencedResources(std::ifstream& ifs) override
		{
			Common::ResourcePack<FTSound>* sounds = ResourceManager::GetInstance()->GetSounds();
			if (!sounds)
				return;
			HMODULE self = GetModuleHandleA("Sound.dll");
			sounds->LoadReferencedResourcesFromChunk(ifs, self, nullptr);
		}
	};
#endif

	SoLoud::Soloud* GetSoloud()
	{
		SoundPlugin* plugin = SoundPlugin::GetInstance();
		if (!plugin)
			return nullptr;
		return plugin->Engine();
	}

	void RegisterOwnedComponent(Common::IComponent* comp)
	{
		if (SoundPlugin* plugin = SoundPlugin::GetInstance())
			plugin->RegisterComponent(comp);
	}

	void UnregisterOwnedComponent(Common::IComponent* comp)
	{
		if (SoundPlugin* plugin = SoundPlugin::GetInstance())
			plugin->UnregisterComponent(comp);
	}

	const char* SoundPlugin::GetName()
	{
		return "Sound";
	}

	void* SoundPlugin::QueryInterface(const char* name) noexcept
	{
#ifdef FOXTROT_EDITOR
		if (name && std::strcmp(name, Core::IEditorResourceService::INTERFACE_NAME) == 0)
			return static_cast<Core::IEditorResourceService*>(mEditorResources);
#else
		(void)name;
#endif
		return nullptr;
	}

	void SoundPlugin::RegisterComponent(Common::IComponent* comp)
	{
		if (mRegisteredComps && comp)
			mRegisteredComps->PushBack(comp);
	}

	void SoundPlugin::UnregisterComponent(Common::IComponent* comp)
	{
		if (!mRegisteredComps || !comp)
			return;

		for (size_t index = mRegisteredComps->GetSize(); 0 < index; --index)
		{
			if (mRegisteredComps->At(index - 1) != comp)
				continue;
			mRegisteredComps->Erase(index - 1);
		}
	}

	void SoundPlugin::ClearRegisteredComponents()
	{
		if (mRegisteredComps)
			mRegisteredComps->Clear();
	}

	void SoundPlugin::SaveProperties(std::ofstream& ofs)
	{
		(void)ofs;
	}

	void SoundPlugin::LoadProperties(std::ifstream& ifs)
	{
		LoadResourceData(ifs);
	}

	void SoundPlugin::StartEngine()
	{
		if (mEngineReady)
			return;
		if (!mEngine)
			mEngine = DBG_NEW SoLoud::Soloud();

		const SoLoud::result started = mEngine->init();
		mEngineReady				  = started == SoLoud::SO_NO_ERROR;
		if (!mEngineReady)
			Common::Debug::LogError(__LINE__, __FILE__, "Failed to start SoLoud");
	}

	void SoundPlugin::StopEngine()
	{
		if (!mEngine)
			return;
		if (mEngineReady)
			mEngine->deinit();
		mEngineReady = false;
	}

	void SoundPlugin::Initialize()
	{
		StartEngine();
	}

	void SoundPlugin::InitializeComponents()
	{
		if (!mRegisteredComps)
			return;
		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!*iter)
				continue;
			(*iter)->Initialize();
		}
	}

	void SoundPlugin::Setup()
	{
		if (!mRegisteredComps)
			return;
		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!*iter)
				continue;
			(*iter)->Setup();
		}
	}

	void SoundPlugin::ProcessInput()
	{
	}

	void SoundPlugin::Update(float deltaTime)
	{
		if (!Core::FTCore::GetInstance()->GetIsUpdating() || !mRegisteredComps)
			return;

		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!*iter || !(*iter)->GetOwner() || !(*iter)->GetOwner()->GetIsActive())
				continue;
			(*iter)->Update(deltaTime);
		}
	}

	void SoundPlugin::LateUpdate(float deltaTime)
	{
		if (!Core::FTCore::GetInstance()->GetIsUpdating() || !mRegisteredComps)
			return;

		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!*iter || !(*iter)->GetOwner() || !(*iter)->GetOwner()->GetIsActive())
				continue;
			(*iter)->LateUpdate(deltaTime);
		}
	}

	void SoundPlugin::Render()
	{
	}

	void SoundPlugin::ProcessEvent()
	{
	}

	void SoundPlugin::ShutDown()
	{
		StopEngine();
		ResourceManager::Destroy();
	}

	SoundPlugin::SoundPlugin(const char* name)
		: mRegisteredComps(DBG_NEW Common::FTDS::DynamicArray<Common::IComponent*>)
		, mEngine(nullptr)
		, mEngineReady(false)
	{
		(void)name;
		mInstance = this;
		ResourceManager::GetInstance();
#ifdef FOXTROT_EDITOR
		mEditorResources = DBG_NEW SoundEditorResourceService;
#endif
	}

	SoundPlugin::~SoundPlugin()
	{
		StopEngine();
		delete mEngine;
		mEngine = nullptr;
		ResourceManager::Destroy();
#ifdef FOXTROT_EDITOR
		delete mEditorResources;
		mEditorResources = nullptr;
#endif
		delete mRegisteredComps;
		mRegisteredComps = nullptr;
		if (mInstance == this)
			mInstance = nullptr;
	}

	SoundPlugin* SoundPlugin::GetInstance()
	{
		return mInstance;
	}

	SoLoud::Soloud* SoundPlugin::Engine() const
	{
		return mEngineReady ? mEngine : nullptr;
	}

	void SoundPlugin::LoadManagerData(std::ifstream& ifs)
	{
		(void)ifs;
	}

	void SoundPlugin::LoadResourceData(std::ifstream& ifs)
	{
		ResourceManager::GetInstance()->LoadResourcesFromChunk(ifs);
	}
} // namespace Sound

namespace
{
	template <typename Component>
	Common::IComponent* CreateComponent(Common::IActor* actor)
	{
		Common::IComponent* comp = CreateRegisteredComponent<Component>(actor);
		if (!comp)
			return nullptr;
		Sound::SoundPlugin* plugin = Sound::SoundPlugin::GetInstance();
		if (plugin)
			plugin->RegisterComponent(comp);
		return comp;
	}
} // namespace

FT_EXPORT void SetComponentFactory(Common::FTDS::HashMap<ComponentFactoryEntry>& entries) noexcept
{
	entries.Insert(Sound::ChunkKey::AudioPlayer::NAME, CreateComponent<Sound::AudioPlayer>);
}

FT_EXPORT void SetResourceFactory(Common::FTDS::HashMap<ResourceFactoryEntry>& entries) noexcept
{
	entries.Insert(Sound::FTSound::GetName(), CreateRegisteredResource<Sound::FTSound>);
}

extern "C"
{
	SOUND_API Common::IPlugin* CreatePlugin(const char* name)
	{
		return DBG_NEW Sound::SoundPlugin(name);
	}

	SOUND_API Common::IComponent* CreateComponent(Common::IPlugin* plugin, Common::IActor* actor, Common::FTDS::String& name)
	{
		(void)plugin;
		Common::FTDS::HashMap<ComponentFactoryEntry> entries(8);
		SetComponentFactory(entries);
		Common::FTDS::Record<ComponentFactoryEntry>* record = entries.At(name);
		if (!record || !record->Value())
			return nullptr;
		return record->Value()(actor);
	}

	SOUND_API void Sound_SetEditorGuiContext(void* context)
	{
#ifdef FOXTROT_EDITOR
		ImGui::SetCurrentContext(static_cast<ImGuiContext*>(context));
#else
		(void)context;
#endif
	}
}
