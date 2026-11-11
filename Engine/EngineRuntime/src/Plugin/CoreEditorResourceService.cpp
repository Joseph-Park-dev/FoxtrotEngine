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
	#include "FTJSON.h"
	#include "FTText.h"
	#include "FTCSV.h"
	#include "ResourceSystem/FTPremade.h"
	#include "FileSystem/FileIOHelper.h"
	#include "FTDS/Static/FTString.h"
	#include "Debugging/DebugFuncs.h"
	#include "FileSystem/DLLPath.h"
	#include "Foxtrot/Core/ModuleApi.h"
	#include "Factory/IFTResourceFactory.h"
	#include "FTDS/Static/HashMap.h"

	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <Windows.h>

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

	/// @brief Reports whether the next chunk line opens a data pack with the supplied key.
	/// @param ifs Input stream positioned at the expected data. The position is restored.
	/// @param key Data-pack key to match.
	/// @return True when the next line is a data pack whose name contains key.
	bool NextDataPackIs(std::ifstream& ifs, const char* key)
	{
		if (!key || !*key)
			return false;
		if (ifs.fail())
			ifs.clear();

		const std::streampos pos = ifs.tellg();
		Common::FTDS::String line;
		const bool			 got = Common::FileIOHelper::GetLine(ifs, line);
		if (ifs.fail())
			ifs.clear();
		ifs.seekg(pos);
		return got && line.Contains(key);
	}

	/// @brief Writes referenced CSV, JSON, text, and premade resources into the chunk stream.
	/// @param ofs Output stream receiving the serialized data.
	void CoreEditorResourceService::SaveReferencedResources(std::ofstream& ofs)
	{
		ResourceManager::GetInstance()->GetCSVs()->SaveReferencedResourcesToChunk(ofs);
		ResourceManager::GetInstance()->GetJSONs()->SaveReferencedResourcesToChunk(ofs);
		ResourceManager::GetInstance()->GetTexts()->SaveReferencedResourcesToChunk(ofs);
		// Written last so the reversed chunk buffer reads this pack first.
		ResourceManager::GetInstance()->GetPremades()->SaveReferencedResourcesToChunk(ofs);
	}

	void CoreEditorResourceService::LoadReferencedResources(std::ifstream& ifs)
	{
		HMODULE module = GetModuleHandleA(Common::DLLPath::CORE);
		// Older chunks have no premade pack. Skip it when the next pack is text.
		if (NextDataPackIs(ifs, FTPremade::GetName()))
			ResourceManager::GetInstance()->GetPremades()->LoadReferencedResourcesFromChunk(ifs, module, nullptr);
		ResourceManager::GetInstance()->GetTexts()->LoadReferencedResourcesFromChunk(ifs, module, nullptr);
		ResourceManager::GetInstance()->GetJSONs()->LoadReferencedResourcesFromChunk(ifs, module, nullptr);
		ResourceManager::GetInstance()->GetCSVs()->LoadReferencedResourcesFromChunk(ifs, module, nullptr);
	}

	class CorePlugin :
		public Common::IPlugin
	{
	public:
		const char* GetName() override { return "Core"; }

	public:
		void  RegisterComponent(Common::IComponent*) override {}
		void  SaveProperties(std::ofstream&) override {}
		void  LoadProperties(std::ifstream&) override {}
		void  Initialize() override;
		/// @brief Calls Initialize on every component registered to this plugin.
		void  InitializeComponents() override {}
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
		if (!mEditorResources)
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

/// @brief Registers CSV, JSON, text, and premade constructors exported by this module.
/// @param entries Receives one factory per core resource pack key.
FT_EXPORT void SetResourceFactory(Common::FTDS::HashMap<ResourceFactoryEntry>& entries) noexcept
{
	entries.Insert(GenericData::FTCSV::GetName(), CreateRegisteredResource<GenericData::FTCSV>);
	entries.Insert(GenericData::FTJSON::GetName(), CreateRegisteredResource<GenericData::FTJSON>);
	entries.Insert(GenericData::FTText::GetName(), CreateRegisteredResource<GenericData::FTText>);
	entries.Insert(Core::FTPremade::GetName(), CreateRegisteredResource<Core::FTPremade>);
}
#else
	#include "Foxtrot/Core/ModuleApi.h"
	#include "Factory/IFTResourceFactory.h"
	#include "FTDS/Static/HashMap.h"

/// @brief Editor resource factories are unused by non-editor builds of this module.
/// @param entries Receives one factory per core resource pack key.
FT_EXPORT void SetResourceFactory(Common::FTDS::HashMap<ResourceFactoryEntry>&) noexcept
{
}
#endif // FOXTROT_EDITOR
