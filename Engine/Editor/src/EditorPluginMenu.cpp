#include "EditorLayer.h"

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>

#include <Windows.h>

#include "ComponentRegistry.h"
#include "Factory/IComponentFactory.h"
#include "FTDS/Static/HashMap.h"
#include "Manager/PluginManager.h"
#include "Plugin/IEditorResourceService.h"
#include "Foxtrot/Core/ModuleApi.h"

namespace
{
	bool IsPinnedModule(const char* name)
	{
		return !name || std::strcmp(name, "Core") == 0 || std::strcmp(name, "EngineRuntime") == 0 || std::strcmp(name, "Editor") == 0 || std::strcmp(name, "FoxtrotD3D11") == 0 || std::strcmp(name, "D3D11") == 0;
	}

	bool ExportNameEquals(const uint8_t* base, size_t size, const uint8_t* name, const char* expected)
	{
		if (!base || !name || !expected || name < base || name >= base + size)
			return false;
		const size_t available	 = static_cast<size_t>((base + size) - name);
		const size_t expectedLen = std::strlen(expected);
		if (available <= expectedLen)
			return false;
		return std::memcmp(name, expected, expectedLen) == 0 && name[expectedLen] == '\0';
	}

	bool ExportsPluginEntry(const std::wstring& path)
	{
		HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (file == INVALID_HANDLE_VALUE)
			return false;

		LARGE_INTEGER fileSize{};
		if (!GetFileSizeEx(file, &fileSize) || fileSize.QuadPart < static_cast<LONGLONG>(sizeof(IMAGE_DOS_HEADER)) || fileSize.QuadPart > 128ll * 1024ll * 1024ll)
		{
			CloseHandle(file);
			return false;
		}

		HANDLE mapping = CreateFileMappingW(file, nullptr, PAGE_READONLY, 0, 0, nullptr);
		if (!mapping)
		{
			CloseHandle(file);
			return false;
		}
		void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
		if (!view)
		{
			CloseHandle(mapping);
			CloseHandle(file);
			return false;
		}

		bool		found = false;
		const auto* base  = static_cast<const uint8_t*>(view);
		const auto* dos	  = static_cast<const IMAGE_DOS_HEADER*>(view);
		const auto	size  = static_cast<size_t>(fileSize.QuadPart);
		if (dos->e_magic == IMAGE_DOS_SIGNATURE && dos->e_lfanew > 0 && static_cast<size_t>(dos->e_lfanew) + sizeof(IMAGE_NT_HEADERS) <= size)
		{
			const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
			if (nt->Signature == IMAGE_NT_SIGNATURE && nt->FileHeader.NumberOfSections > 0 && nt->FileHeader.NumberOfSections < 96)
			{
				const IMAGE_DATA_DIRECTORY& directory = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
				auto						rvaToPtr  = [&](DWORD rva) -> const uint8_t* {
					const IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);
					for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i)
					{
						DWORD span = section[i].Misc.VirtualSize;
						if (span < section[i].SizeOfRawData)
							span = section[i].SizeOfRawData;
						if (rva < section[i].VirtualAddress || rva >= section[i].VirtualAddress + span)
							continue;
						const size_t offset = static_cast<size_t>(section[i].PointerToRawData) + (rva - section[i].VirtualAddress);
						if (offset >= size)
							return nullptr;
						return base + offset;
					}
					return nullptr;
				};

				if (directory.VirtualAddress != 0)
				{
					const auto* exports = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(rvaToPtr(directory.VirtualAddress));
					const auto* names	= exports ? reinterpret_cast<const DWORD*>(rvaToPtr(exports->AddressOfNames)) : nullptr;
					if (exports && names)
					{
						const DWORD count = exports->NumberOfNames < 8192 ? exports->NumberOfNames : 8192;
						for (DWORD i = 0; i < count; ++i)
						{
							const auto* name = rvaToPtr(names[i]);
							if (ExportNameEquals(base, size, name, "FtGetModuleAPI") || ExportNameEquals(base, size, name, "CreatePlugin"))
							{
								found = true;
								break;
							}
						}
					}
				}
			}
		}

		UnmapViewOfFile(view);
		CloseHandle(mapping);
		CloseHandle(file);
		return found;
	}

	void ReleasePaths(Common::FTDS::DynamicArray<Common::FTDS::String*>& paths)
	{
		for (size_t i = 0; i < paths.GetSize(); ++i)
			delete paths.At(i);
		paths.Clear();
	}

	void DllNameForPlugin(const char* name, Common::FTDS::String& out)
	{
		if (!name)
		{
			out.Assign("");
			return;
		}
		if (std::strcmp(name, "Core") == 0)
		{
			out.Assign("EngineRuntime.dll");
			return;
		}
		if (std::strcmp(name, "D3D11") == 0 || std::strcmp(name, "FoxtrotD3D11") == 0)
		{
			out.Assign("FoxtrotD3D11.dll");
			return;
		}
		out.Assign(name);
		out.Append(".dll");
	}

	/// @brief Starts script/build-game.bat without waiting, so the editor can answer its unload and reload requests.
	/// @return True when a new build process was started. False when one is already running or the script could not be started.
	bool LaunchBuildGameBat(bool& alreadyRunning)
	{
		alreadyRunning			   = false;
		static HANDLE buildProcess = nullptr;
		if (buildProcess)
		{
			if (WaitForSingleObject(buildProcess, 0) == WAIT_TIMEOUT)
			{
				alreadyRunning = true;
				return false;
			}
			CloseHandle(buildProcess);
			buildProcess = nullptr;
		}

		wchar_t		exePath[32768]{};
		const DWORD count = GetModuleFileNameW(nullptr, exePath, 32768);
		if (count == 0 || count >= 32768)
			return false;

		std::error_code		  error;
		std::filesystem::path directory = std::filesystem::path(exePath).parent_path();
		std::filesystem::path bat;
		for (int depth = 0; depth < 8 && !directory.empty(); ++depth)
		{
			const std::filesystem::path candidate = directory / L"script" / L"build-game.bat";
			if (std::filesystem::is_regular_file(candidate, error) && !error)
			{
				bat = candidate;
				break;
			}
			const std::filesystem::path parent = directory.parent_path();
			if (parent == directory)
				break;
			directory = parent;
		}
		if (bat.empty())
			return false;

		std::wstring command = L"cmd.exe /c \"";
		command += bat.wstring();
		command += L"\"";

		STARTUPINFOW startup{};
		startup.cb = sizeof(startup);
		PROCESS_INFORMATION process{};
		if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, CREATE_NEW_CONSOLE, nullptr, nullptr, &startup, &process))
			return false;

		CloseHandle(process.hThread);
		buildProcess = process.hProcess;
		return true;
	}
} // namespace

namespace Editor
{
	struct EditorPluginModule
	{
		Common::FTDS::String SourcePath;
		Common::FTDS::String FileName;
		Common::FTDS::String ModuleName;
		HMODULE				 Handle;
		Foxtrot::ModuleAPI	 Api;
		bool				 Initialized;

		EditorPluginModule()
			: Handle(nullptr)
			, Api{}
			, Initialized(false)
		{
		}
	};

	void UnloadModule(EditorPluginModule* module)
	{
		if (!module)
			return;
		if (!module->ModuleName.IsEmpty())
			Core::PluginManager::GetInstance()->Detach(module->ModuleName.C_Str());
		if (module->Initialized && module->Api.Destroy)
			module->Api.Destroy(module->Api.Instance);
		module->Initialized = false;
		module->Api			= {};
		if (module->Handle)
		{
			FreeLibrary(module->Handle);
			module->Handle = nullptr;
		}
	}

	void EditorLayer::RefreshGamePlugin()
	{
		bool alreadyRunning = false;
		if (LaunchBuildGameBat(alreadyRunning))
			mPluginStatus.Assign("Building Game.dll...");
		else if (alreadyRunning)
			mPluginStatus.Assign("Game.dll build is already running.");
		else
			mPluginStatus.Assign("Failed to start script\\build-game.bat.");
	}

	/// @brief Opens the plugin manager and lists available and loaded plugin DLLs.
	void EditorLayer::DisplayPluginManagerMenu()
	{
		if (ImGui::Button("Plugin Manager"))
			mPluginManagerOpen = true;
		if (!mPluginManagerOpen)
			return;

		ImGui::SetNextWindowSize(ImVec2(560.f, 480.f), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin("Plugin Manager", &mPluginManagerOpen))
		{
			ImGui::End();
			return;
		}

		const char* projectPath = mGetProjPathFunc && mGetProjPathFunc() ? mGetProjPathFunc()->C_Str() : "";
		ImGui::TextUnformatted("Project directory");
		ImGui::TextWrapped("%s", projectPath && *projectPath ? projectPath : "(none)");
		if (ImGui::Button("Reimport All"))
			ReimportAllPlugins();

		ImGui::Separator();
		ImGui::TextUnformatted("Available");
		Common::FTDS::DynamicArray<Common::FTDS::String*> available(8);
		CollectPluginDlls(available);
		if (available.IsEmpty())
			ImGui::TextDisabled("No plugin DLLs in the project directory.");
		else
		{
			for (size_t i = 0; i < available.GetSize(); ++i)
			{
				std::filesystem::path file(available.At(i)->C_Str());
				ImGui::BulletText("%s", file.filename().string().c_str());
			}
		}
		ReleasePaths(available);

		ImGui::Separator();
		ImGui::TextUnformatted("Loaded");
		Common::FTDS::String reloadName;
		auto*				 plugins   = Core::GetAvailablePlugins();
		bool				 anyLoaded = plugins && !plugins->IsEmpty();
		if (!anyLoaded)
			ImGui::TextDisabled("No plugins are loaded.");
		else if (ImGui::BeginTable("LoadedPlugins", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Plugin");
			ImGui::TableSetupColumn("DLL");
			ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 190.f);
			ImGui::TableHeadersRow();

			Common::FTDS::String unloadName;
			for (auto it = plugins->Begin(); it != plugins->End(); ++it)
			{
				if (!*it || !(*it)->Value())
					continue;
				const char*			 name = (*it)->Key().C_Str();
				Common::FTDS::String dllName;
				DllNameForPlugin(name, dllName);

				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(name);
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(dllName.C_Str());
				ImGui::TableNextColumn();
				ImGui::PushID(name);
				if (ImGui::SmallButton("Hot Reload"))
					reloadName.Assign(name);
				ImGui::SameLine();
				if (ImGui::SmallButton("Unload"))
					unloadName.Assign(name);
				ImGui::PopID();
			}
			ImGui::EndTable();

			if (!unloadName.IsEmpty())
			{
				if (IsPinnedModule(unloadName.C_Str()))
					mPluginStatus.Assign("Cannot unload this plugin while the editor is running.");
				else if (UnloadPlugin(unloadName.C_Str()))
				{
					mPluginStatus.Assign("Unloaded ");
					mPluginStatus.Append(unloadName.C_Str());
					mPluginStatus.Append(".");
				}
				else
				{
					mPluginStatus.Assign("Failed to unload ");
					mPluginStatus.Append(unloadName.C_Str());
					mPluginStatus.Append(".");
				}
			}
		}

		if (!reloadName.IsEmpty())
		{
			if (IsPinnedModule(reloadName.C_Str()))
				mPluginStatus.Assign("Cannot hot reload this plugin while the editor is running.");
			else if (reloadName.Equal("Game"))
				RefreshGamePlugin();
			else if (HotReloadPlugin(reloadName.C_Str()))
			{
				mPluginStatus.Assign("Hot reloaded ");
				mPluginStatus.Append(reloadName.C_Str());
				mPluginStatus.Append(".");
			}
			else
			{
				mPluginStatus.Assign("Failed to hot reload ");
				mPluginStatus.Append(reloadName.C_Str());
				mPluginStatus.Append(".");
			}
		}

		if (!mPluginStatus.IsEmpty())
		{
			ImGui::Separator();
			ImGui::TextWrapped("%s", mPluginStatus.C_Str());
		}

		ImGui::End();
	}

	/// @brief Loads every plugin DLL in the project directory, reloading ones already imported.
	void EditorLayer::ReimportAllPlugins()
	{
		Common::FTDS::DynamicArray<Common::FTDS::String*> paths(8);
		CollectPluginDlls(paths);

		int imported = 0;
		int skipped	 = 0;
		int failed	 = 0;
		for (size_t i = 0; i < paths.GetSize(); ++i)
		{
			std::filesystem::path file(paths.At(i)->C_Str());
			const std::string	  stem = file.stem().string();
			if (IsPinnedModule(stem.c_str()))
			{
				++skipped;
				continue;
			}
			if (HotReloadPlugin(stem.c_str()))
				++imported;
			else
				++failed;
		}
		ReleasePaths(paths);

		char message[192];
		sprintf_s(message, "Reimport finished. Loaded %d, skipped %d, failed %d.", imported, skipped, failed);
		mPluginStatus.Assign(message);
	}

	/// @brief Unloads or reloads Game.dll when a build script writes a request file.
	void EditorLayer::PollGameDllCommand()
	{
		wchar_t		temp[MAX_PATH]{};
		const DWORD length = GetTempPathW(MAX_PATH, temp);
		if (length == 0 || length >= MAX_PATH)
			return;

		const std::filesystem::path directory = std::filesystem::path(temp) / L"FoxtrotEngine";
		const std::filesystem::path request	  = directory / L"game-dll.req";
		std::error_code				error;
		if (!std::filesystem::is_regular_file(request, error))
			return;

		std::string command;
		{
			std::ifstream input(request);
			if (!input)
				return;
			std::getline(input, command);
		}
		std::filesystem::remove(request, error);

		while (!command.empty() && (command.back() == '\r' || command.back() == ' ' || command.back() == '\t'))
			command.pop_back();

		bool ok = false;
		if (command == "unload")
		{
			ok = UnloadPlugin("Game") || GetModuleHandleA("Game.dll") == nullptr;
			mPluginStatus.Assign(ok ? "Unloaded Game." : "Failed to unload Game.");
		}
		else if (command == "reload")
		{
			ok = HotReloadPlugin("Game");
			mPluginStatus.Assign(ok ? "Reloaded Game." : "Failed to reload Game.");
		}

		const std::filesystem::path pending = directory / L"game-dll.ack.tmp";
		const std::filesystem::path ack		= directory / L"game-dll.ack";
		{
			std::ofstream output(pending, std::ios::trunc);
			if (!output)
				return;
			output << (ok ? "ok" : "fail");
		}
		std::filesystem::remove(ack, error);
		std::filesystem::rename(pending, ack, error);
	}

	/// @brief Unloads one plugin DLL and leaves it unmapped.
	/// @param moduleName Plugin or module name shown in the loaded list.
	/// @return True when the plugin DLL was released.
	bool EditorLayer::UnloadPlugin(const char* moduleName)
	{
		if (!moduleName || IsPinnedModule(moduleName))
			return false;

		const Foxtrot::Status hosted = Core::UnloadPluginModule(moduleName);
		if (hosted == Foxtrot::Status::Ok)
		{
			RefreshAfterPluginLoad();
			return true;
		}
		if (hosted != Foxtrot::Status::InvalidArgument)
			return false;

		if (!UnloadOwnedPlugin(moduleName))
			return false;
		RefreshAfterPluginLoad();
		return true;
	}

	/// @brief Unloads one plugin DLL and loads it again.
	/// @param moduleName Plugin or module name shown in the loaded list.
	/// @return True when the plugin is attached after the reload.
	bool EditorLayer::HotReloadPlugin(const char* moduleName)
	{
		if (!moduleName || IsPinnedModule(moduleName))
			return false;

		const Foxtrot::Status hosted = Core::ReloadPluginModule(moduleName);
		if (hosted == Foxtrot::Status::Ok)
		{
			RefreshAfterPluginLoad();
			return true;
		}
		if (hosted != Foxtrot::Status::InvalidArgument)
			return false;

		Common::FTDS::String path;
		if (OwnsPlugin(moduleName))
		{
			for (size_t i = 0; i < mEditorPlugins->GetSize(); ++i)
			{
				EditorPluginModule* module = mEditorPlugins->At(i);
				if (module && module->ModuleName.Equal(moduleName))
				{
					path.Assign(module->SourcePath);
					break;
				}
			}
			if (path.IsEmpty() || !UnloadOwnedPlugin(moduleName))
				return false;
		}
		else if (!FindPluginFile(moduleName, path))
			return false;

		if (!LoadPluginFile(path.C_Str()))
			return false;
		RefreshAfterPluginLoad();
		return true;
	}

	/// @brief Unloads plugin DLLs imported by the editor.
	void EditorLayer::ReleaseEditorPlugins()
	{
		if (!mEditorPlugins)
			return;
		while (!mEditorPlugins->IsEmpty())
		{
			const size_t		index  = mEditorPlugins->GetSize() - 1;
			EditorPluginModule* module = mEditorPlugins->At(index);
			UnloadModule(module);
			delete module;
			mEditorPlugins->PopBack();
		}
	}

	/// @brief Appends plugin DLL paths found directly in the project directory.
	/// @param paths Receives owned path strings. The caller deletes each entry.
	void EditorLayer::CollectPluginDlls(Common::FTDS::DynamicArray<Common::FTDS::String*>& paths) const
	{
		if (!mGetProjPathFunc || !mGetProjPathFunc() || mGetProjPathFunc()->IsEmpty())
			return;

		Common::FTDS::String path = mGetProjPathFunc()->C_Str();
		path.Append("\\Plugins");
		const std::filesystem::path directory(path.C_Str());
		std::error_code				error;
		if (!std::filesystem::is_directory(directory, error))
			return;

		try
		{
			for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(directory))
			{
				if (!entry.is_regular_file())
					continue;
				const std::wstring extension = entry.path().extension().wstring();
				if (_wcsicmp(extension.c_str(), L".dll") != 0)
					continue;
				if (!ExportsPluginEntry(entry.path().wstring()))
					continue;
				// DynamicArray keeps raw slots. A String value has no constructed vtable there, so PushBack crashes.
				paths.PushBack(DBG_NEW Common::FTDS::String(entry.path().string().c_str()));
			}
		}
		catch (const std::filesystem::filesystem_error&)
		{
		}
	}

	/// @brief Loads one plugin DLL that the executable has not already loaded.
	/// @param path Filesystem path of the DLL.
	/// @return True when the plugin was attached.
	bool EditorLayer::LoadPluginFile(const char* path)
	{
		if (!path || !*path || !mEditorPlugins)
			return false;

		const std::filesystem::path file(path);
		const std::string			fileName = file.filename().string();
		if (GetModuleHandleA(fileName.c_str()))
			return false;

		HMODULE handle = LoadLibraryExW(file.wstring().c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
		if (!handle)
			return false;

		auto get = reinterpret_cast<Foxtrot::GetModuleAPI>(GetProcAddress(handle, "FtGetModuleAPI"));
		if (!get)
		{
			FreeLibrary(handle);
			return false;
		}

		EditorPluginModule* module = DBG_NEW EditorPluginModule();
		module->Handle			   = handle;
		module->SourcePath.Assign(path);
		module->FileName.Assign(fileName.c_str());
		if (get(Foxtrot::ModuleAbi, sizeof(module->Api), &module->Api) != Foxtrot::Status::Ok || !module->Api.Initialize || !module->Api.Shutdown || !module->Api.Destroy || !module->Api.Query || !module->Api.Name || IsPinnedModule(module->Api.Name))
		{
			if (module->Api.Destroy)
				module->Api.Destroy(module->Api.Instance);
			FreeLibrary(handle);
			delete module;
			return false;
		}

		if (module->Api.Initialize(module->Api.Instance, nullptr) != Foxtrot::Status::Ok)
		{
			module->Api.Destroy(module->Api.Instance);
			FreeLibrary(handle);
			delete module;
			return false;
		}
		module->Initialized = true;

		void* plugin = module->Api.Query(module->Api.Instance, "LegacyPlugin");
		if (!plugin || Core::PluginManager::GetInstance()->GetPlugin(module->Api.Name))
		{
			module->Api.Destroy(module->Api.Instance);
			FreeLibrary(handle);
			delete module;
			return false;
		}

		module->ModuleName.Assign(module->Api.Name);
		Core::PluginManager::GetInstance()->Attach(module->ModuleName.C_Str(), static_cast<Common::IPlugin*>(plugin));
		mEditorPlugins->PushBack(module);
		return true;
	}

	/// @brief Unloads one plugin DLL previously loaded by the editor.
	/// @param moduleName Plugin or module name.
	/// @return True when an editor-owned plugin of that name was released.
	bool EditorLayer::UnloadOwnedPlugin(const char* moduleName)
	{
		if (!mEditorPlugins || !moduleName)
			return false;
		for (size_t i = 0; i < mEditorPlugins->GetSize(); ++i)
		{
			EditorPluginModule* module = mEditorPlugins->At(i);
			if (!module || !module->ModuleName.Equal(moduleName))
				continue;
			UnloadModule(module);
			delete module;
			const size_t last	  = mEditorPlugins->GetSize() - 1;
			mEditorPlugins->At(i) = mEditorPlugins->At(last);
			mEditorPlugins->PopBack();
			return true;
		}
		return false;
	}

	/// @brief Tests whether the editor loaded this plugin DLL.
	/// @param moduleName Plugin or module name.
	/// @return True when the editor owns the loaded module.
	bool EditorLayer::OwnsPlugin(const char* moduleName) const
	{
		if (!mEditorPlugins || !moduleName)
			return false;
		for (size_t i = 0; i < mEditorPlugins->GetSize(); ++i)
		{
			EditorPluginModule* module = mEditorPlugins->At(i);
			if (module && module->ModuleName.Equal(moduleName))
				return true;
		}
		return false;
	}

	/// @brief Resolves a plugin name to a DLL path in the project directory.
	/// @param moduleName Plugin or module name.
	/// @param path Receives the matching DLL path.
	/// @return True when a matching DLL is present.
	bool EditorLayer::FindPluginFile(const char* moduleName, Common::FTDS::String& path) const
	{
		if (!moduleName)
			return false;
		Common::FTDS::DynamicArray<Common::FTDS::String*> paths(8);
		CollectPluginDlls(paths);
		bool found = false;
		for (size_t i = 0; i < paths.GetSize(); ++i)
		{
			std::filesystem::path file(paths.At(i)->C_Str());
			if (file.stem().string() == moduleName)
			{
				path.Assign(paths.At(i)->C_Str());
				found = true;
				break;
			}
		}
		ReleasePaths(paths);
		return found;
	}

	/// @brief Refreshes component factories and editor services after a plugin load.
	void EditorLayer::RefreshAfterPluginLoad()
	{
		ComponentRegistry::GetInstance()->FetchFromImportedDLLs();
		if (mEditorPlugins)
		{
			using SetComponentFactoryFn = void(__cdecl*)(Common::FTDS::HashMap<ComponentFactoryEntry>&);
			for (size_t i = 0; i < mEditorPlugins->GetSize(); ++i)
			{
				EditorPluginModule* module = mEditorPlugins->At(i);
				if (!module || !module->Handle)
					continue;
				auto setFactory = reinterpret_cast<SetComponentFactoryFn>(GetProcAddress(module->Handle, "SetComponentFactory"));
				if (setFactory)
					setFactory(ComponentRegistry::GetInstance()->GetComponentFactories());
			}
		}

		if (!mResService)
			return;
		mResService->Clear();
		auto* plugins = Core::GetAvailablePlugins();
		if (!plugins)
			return;
		for (auto it = plugins->Begin(); it != plugins->End(); ++it)
		{
			if (!*it || !(*it)->Value())
				continue;
			void* service = (*it)->Value()->QueryInterface(Core::IEditorResourceService::INTERFACE_NAME);
			if (service)
				mResService->PushBack(static_cast<Core::IEditorResourceService*>(service));
		}
	}
} // namespace Editor
