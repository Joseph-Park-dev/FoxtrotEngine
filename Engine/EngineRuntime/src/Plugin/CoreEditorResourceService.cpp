// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Plugin/CoreEditorResourceService.h"

#ifdef FOXTROT_EDITOR
	#include <cstring>
	#include <fstream>

	#include "Plugin/IPlugin.h"
	#include "Manager/ResourceManager.h"
	#include "FTDS/Static/FTString.h"
	#include "Debugging/DebugFuncs.h"

namespace Core
{
	bool CoreEditorResourceService::LoadDefaultResources(const char* projDirectory, Graphics::IRenderer* renderer) noexcept
	{
		return true;
	}

	bool CoreEditorResourceService::LoadAllResourcesInAsset(
		const char*			 assetDirectory,
		Graphics::IRenderer* renderer) noexcept
	{
		(void)renderer;
		if (!assetDirectory || !*assetDirectory)
			return false;

		try
		{
			Common::FTDS::String directory(assetDirectory);

			ResourceManager::GetInstance()
				->LoadAllResourcesInAsset(&directory);

			return true;
		}
		catch (...)
		{
			Common::Debug::LogError(
				__LINE__, __FILE__, "Asset loading failed.");
			return false;
		}
	}

	void CoreEditorResourceService::DisplayLoadedResources()
	{
		ResourceManager::GetInstance()->EditorUIUpdate();
	}

	class CorePlugin :
		public Common::IPlugin
	{
	public:
		void  RegisterComponent(Common::IComponent*) override {}
		void  SaveProperties(std::ofstream&) override {}
		void  LoadProperties(std::ifstream&) override {}
		void  Initialize() override;
		void  Setup() override {}
		void  ProcessInput() override {}
		void  Update(float) override {}
		void  LateUpdate(float) override {}
		void  Render() override {}
		void  ProcessEvent() override {}
		void  ShutDown() override {}
		void* QueryInterface(const char* name) noexcept override;

		~CorePlugin() override;

	protected:
		void LoadManagerData(std::ifstream&) override {}
		void LoadResourceData(std::ifstream&) override {}

	private:
		CoreEditorResourceService* mEditorResources = nullptr;
	};

	void CorePlugin::Initialize()
	{
		mEditorResources = DBG_NEW CoreEditorResourceService;
	}

	void* CorePlugin::QueryInterface(const char* name) noexcept
	{
		if (std::strcmp(name, IEditorResourceService::INTERFACE_NAME) == 0)
			return static_cast<IEditorResourceService*>(mEditorResources);
		return nullptr;
	}

	CorePlugin::~CorePlugin()
	{
		delete mEditorResources;
	}

	Common::IPlugin* CreateCorePlugin()
	{
		return DBG_NEW CorePlugin;
	}
} // namespace Core
#endif // FOXTROT_EDITOR
