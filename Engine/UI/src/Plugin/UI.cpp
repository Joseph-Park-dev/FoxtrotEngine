// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "pch.h"
#include "Plugin/UI.h"

#include <fstream>

#include "Plugin/UIExports.h"
#include "Foxtrot/Core/ModuleApi.h"
#include "Plugin/IPlugin.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "FTDS/Static/HashMap.h"
#include "Factory/IComponentFactory.h"
#include "Factory/IFTResourceFactory.h"
#include "Actor/IActor.h"
#include "Actor/ActorData.h"
#include "Core/FTCore.h"
#include "Manager/PluginManager.h"
#include "Renderer/IRenderer.h"
#include "Renderer/ICamera.h"
#include "IInputDevice.h"
#include "FileSystem/FileTypes.h"
#include "ResourceSystem/Font/FTFont.h"
#include "ResourceSystem/ResourcePack.h"
#include "Component/Button.h"
#include "Component/UIComponent.h"
#include "Debugging/DebugMemAlloc.h"

#ifdef FOXTROT_EDITOR
	#include "Plugin/ICommandHistory.h"
	#include "Plugin/GetFunc.h"
	#include "Plugin/IEditorResourceService.h"
	#include "FileSystem/DLLPath.h"
	#include <imgui.h>
	#include <cstring>
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
			"GetCMDHistory")();
	}

	ICommandHistory* GetCommandHistory()
	{
		if (!sCommandHistory)
			AcquireCommandHistory();
		return sCommandHistory;
	}
} // namespace Editor
#endif

namespace UI
{
#ifdef FOXTROT_EDITOR
	class UIEditorResourceService;
#endif

	class UIPlugin : public Common::IPlugin
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

		void DrawComponents(Graphics::IRenderer* renderer, Graphics::ICamera* camera);

		explicit UIPlugin(const char* name);
		~UIPlugin() override;

		static UIPlugin* GetInstance();

	protected:
		void LoadManagerData(std::ifstream& ifs) override;
		void LoadResourceData(std::ifstream& ifs) override;

	private:
		void ResolveFrameDevices();

		Common::FTDS::DynamicArray<Common::IComponent*>* mRegisteredComps;
#ifdef FOXTROT_EDITOR
		UIEditorResourceService* mEditorResources = nullptr;
#endif
		static UIPlugin* mInstance;
	};

	UIPlugin* UIPlugin::mInstance = nullptr;
	static Common::ResourcePack<D3D11::FTFont>* sFonts	  = nullptr;
	static Graphics::IRenderer*					sRenderer = nullptr;
	static Graphics::ICamera*					sCamera	  = nullptr;
	static InputSystem::IInputDevice*			sInput	  = nullptr;

#ifdef FOXTROT_EDITOR
	class UIEditorResourceService final :
		public Core::IEditorResourceService
	{
	public:
		bool LoadDefaultResources(const char*, Graphics::IRenderer* renderer) noexcept override
		{
			if (renderer)
				mRenderer = renderer;
			return true;
		}

		bool LoadAllResourcesInAsset(const char* assetDirectory, Graphics::IRenderer* renderer) noexcept override
		{
			if (renderer)
				mRenderer = renderer;
			Common::ResourcePack<D3D11::FTFont>* fonts = GetFonts();
			if (!fonts || !assetDirectory || !*assetDirectory)
				return false;
			fonts->LoadAllResourcesInAsset(assetDirectory, Common::FileTypes::FONT, mRenderer);
			return true;
		}

		void DisplayLoadedResources() override
		{
			if (Common::ResourcePack<D3D11::FTFont>* fonts = GetFonts())
				fonts->DisplayLoadedResources("Fonts");
		}

		void SaveReferencedResources(std::ofstream& ofs) override
		{
			if (Common::ResourcePack<D3D11::FTFont>* fonts = GetFonts())
				fonts->SaveReferencedResourcesToChunk(ofs);
		}

		void LoadReferencedResources(std::ifstream& ifs) override
		{
			Common::ResourcePack<D3D11::FTFont>* fonts = GetFonts();
			if (!fonts)
				return;
			HMODULE self = GetModuleHandleA("UI.dll");
			fonts->LoadReferencedResourcesFromChunk(ifs, self, mRenderer);
		}

	private:
		Graphics::IRenderer* mRenderer = nullptr;
	};
#endif

	UIPlugin* GetUI()
	{
		return UIPlugin::GetInstance();
	}

	Common::ResourcePack<D3D11::FTFont>* GetFonts()
	{
		return sFonts;
	}

	Graphics::ICamera* GetUICamera()
	{
		return sCamera;
	}

	InputSystem::IInputDevice* GetUIInputDevice()
	{
		return sInput;
	}

	void RegisterOwnedComponent(Common::IComponent* comp)
	{
		if (UIPlugin* plugin = UIPlugin::GetInstance())
			plugin->RegisterComponent(comp);
	}

	void UnregisterOwnedComponent(Common::IComponent* comp)
	{
		if (UIPlugin* plugin = UIPlugin::GetInstance())
			plugin->UnregisterComponent(comp);
	}

	const char* UIPlugin::GetName()
	{
		return "UI";
	}

	void* UIPlugin::QueryInterface(const char* name) noexcept
	{
#ifdef FOXTROT_EDITOR
		if (name && std::strcmp(name, Core::IEditorResourceService::INTERFACE_NAME) == 0)
			return static_cast<Core::IEditorResourceService*>(mEditorResources);
#else
		(void)name;
#endif
		return nullptr;
	}

	void UIPlugin::RegisterComponent(Common::IComponent* comp)
	{
		if (mRegisteredComps && comp)
			mRegisteredComps->PushBack(comp);
	}

	void UIPlugin::UnregisterComponent(Common::IComponent* comp)
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

	void UIPlugin::ClearRegisteredComponents()
	{
		if (mRegisteredComps)
			mRegisteredComps->Clear();
	}

	void UIPlugin::SaveProperties(std::ofstream& ofs)
	{
		(void)ofs;
	}

	void UIPlugin::LoadProperties(std::ifstream& ifs)
	{
		(void)ifs;
	}

	void UIPlugin::Initialize()
	{
	}

	void UIPlugin::InitializeComponents()
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

	void UIPlugin::Setup()
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

	void UIPlugin::ResolveFrameDevices()
	{
		sRenderer = nullptr;
		sCamera	  = nullptr;
		sInput	  = nullptr;
		Common::FTDS::HashMap<Common::IPlugin*>* plugins = Core::GetAvailablePlugins();
		if (!plugins)
			return;

		Common::FTDS::String					 name("FoxtrotD3D11");
		Common::FTDS::Record<Common::IPlugin*>*	 record	 = plugins->At(name);
		Common::IPlugin*						 graphics = record ? record->Value() : nullptr;
		if (!graphics)
			return;

		sRenderer = static_cast<Graphics::IRenderer*>(graphics->QueryInterface(Graphics::RENDERER_INTERFACE));
		sInput	  = static_cast<InputSystem::IInputDevice*>(graphics->QueryInterface("InputDevice"));
		sCamera	  = static_cast<Graphics::ICamera*>(graphics->QueryInterface(Graphics::GAME_CAMERA_INTERFACE));
	}

	void UIPlugin::ProcessInput()
	{
		ResolveFrameDevices();
		if (!mRegisteredComps)
			return;

		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!*iter || !(*iter)->GetOwner() || !(*iter)->GetOwner()->GetIsActive())
				continue;
			(*iter)->ProcessInput(sInput);
		}
	}

	void UIPlugin::Update(float deltaTime)
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

	void UIPlugin::LateUpdate(float deltaTime)
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

	void UIPlugin::DrawComponents(Graphics::IRenderer* renderer, Graphics::ICamera* camera)
	{
		if (!mRegisteredComps)
			return;
		if (camera)
			sCamera = camera;

		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!*iter || !(*iter)->GetOwner() || !(*iter)->GetOwner()->GetIsActive())
				continue;
			if (Core::IsComponentRenderingSuspended(*iter))
				continue;
			(*iter)->Render(renderer, camera);
		}
	}

	void UIPlugin::Render()
	{
#ifdef FOXTROT_EDITOR
		return;
#else
		ResolveFrameDevices();
		if (!sRenderer)
			return;
		DrawComponents(sRenderer, sCamera);
#endif
	}

	void UIPlugin::ProcessEvent()
	{
	}

	void UIPlugin::ShutDown()
	{
	}

	UIPlugin::UIPlugin(const char* name)
		: mRegisteredComps(DBG_NEW Common::FTDS::DynamicArray<Common::IComponent*>)
	{
		(void)name;
		mInstance = this;
		if (!sFonts)
			sFonts = DBG_NEW Common::ResourcePack<D3D11::FTFont>(16);
#ifdef FOXTROT_EDITOR
		mEditorResources = DBG_NEW UIEditorResourceService;
#endif
	}

	UIPlugin::~UIPlugin()
	{
#ifdef FOXTROT_EDITOR
		delete mEditorResources;
		mEditorResources = nullptr;
#endif
		delete sFonts;
		sFonts = nullptr;
		delete mRegisteredComps;
		if (mInstance == this)
			mInstance = nullptr;
	}

	UIPlugin* UIPlugin::GetInstance()
	{
		return mInstance;
	}

	void UIPlugin::LoadManagerData(std::ifstream& ifs)
	{
		(void)ifs;
	}

	void UIPlugin::LoadResourceData(std::ifstream& ifs)
	{
		(void)ifs;
	}
} // namespace UI

namespace
{
	template <typename Component>
	Common::IComponent* CreateComponent(Common::IActor* actor)
	{
		Common::IComponent* comp = CreateRegisteredComponent<Component>(actor);
		if (!comp)
			return nullptr;
		UI::UIPlugin* plugin = UI::UIPlugin::GetInstance();
		if (plugin)
			plugin->RegisterComponent(comp);
		return comp;
	}
} // namespace

FT_EXPORT void SetComponentFactory(Common::FTDS::HashMap<ComponentFactoryEntry>& entries) noexcept
{
	entries.Insert(UI::ChunkKey::UIComponent::NAME, CreateComponent<UI::UIComponent>);
	entries.Insert(UI::ChunkKey::Button::NAME, CreateComponent<UI::Button>);
}

FT_EXPORT void SetResourceFactory(Common::FTDS::HashMap<ResourceFactoryEntry>& entries) noexcept
{
	entries.Insert(D3D11::FTFont::GetName(), CreateRegisteredResource<D3D11::FTFont>);
}

extern "C"
{
	UI_API Common::IPlugin* CreatePlugin(const char* name)
	{
		return DBG_NEW UI::UIPlugin(name);
	}

	UI_API Common::IComponent* CreateComponent(Common::IPlugin* plugin, Common::IActor* actor, Common::FTDS::String& name)
	{
		(void)plugin;
		Common::FTDS::HashMap<ComponentFactoryEntry> entries(8);
		SetComponentFactory(entries);
		Common::FTDS::Record<ComponentFactoryEntry>* record = entries.At(name);
		if (!record || !record->Value())
			return nullptr;
		return record->Value()(actor);
	}

	UI_API void UI_SetEditorGuiContext(void* context)
	{
#ifdef FOXTROT_EDITOR
		ImGui::SetCurrentContext(static_cast<ImGuiContext*>(context));
#else
		(void)context;
#endif
	}
}
