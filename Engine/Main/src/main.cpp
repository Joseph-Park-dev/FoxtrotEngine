#include <cstdio>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <fstream>

#include "ModuleHost.h"
#include "Foxtrot/Runtime/RuntimeApi.h"
#include "FileSystem/DLLPath.h"
#include "FTDS/Static/Array.h"
#include "FTDS/Static/FTString.h"

namespace
{
	ModuleHost* gModules = nullptr;

	/// @brief Chooses the Game.dll that contains the open project's behaviors.
	/// @return Absolute Plugins/Game.dll when that file exists, otherwise Game.dll beside the executable.
	std::wstring ResolveGameDll()
	{
		const std::wstring fallback = L"Game.dll";
		HMODULE			   runtime	= GetModuleHandleW(L"EngineRuntime.dll");
		if (!runtime)
			return fallback;

		using GetProjectPathFn = Common::FTDS::String*(__cdecl*)();
		auto getPath		   = reinterpret_cast<GetProjectPathFn>(GetProcAddress(runtime, "GetProjectPath"));
		if (!getPath)
			return fallback;

		Common::FTDS::String* path = getPath();
		std::error_code		  error;
		// Game data may not have assigned a project yet. The working directory is that project when the editor is launched from it.
		std::filesystem::path root = (path && path->C_Str() && path->C_Str()[0]) ? std::filesystem::path(path->C_Str()) : std::filesystem::current_path(error);
		if (error || root.empty())
			return fallback;

		std::filesystem::path plugins = root / "Plugins" / "Game.dll";
		if (!std::filesystem::exists(plugins, error))
			return fallback;

		plugins = std::filesystem::absolute(plugins, error);
		if (error || plugins.empty())
			return fallback;
		return plugins.wstring();
	}

	/// @brief Unloads one plugin DLL owned by the active module host.
	/// @param name Module name required by the host's ABI validation.
	/// @return Status reported by the host unload.
	Foxtrot::Status FT_CALL UnloadHostedPlugin(const char* name) noexcept
	{
		if (!gModules || !name)
			return Foxtrot::Status::InvalidArgument;
		try
		{
			return gModules->Unload(name);
		}
		catch (...)
		{
			return Foxtrot::Status::Failed;
		}
	}

	/// @brief Unloads and loads one plugin DLL owned by the active module host.
	/// @param name Module name required by the host's ABI validation.
	/// @return Status reported by the host reload.
	Foxtrot::Status FT_CALL ReloadHostedPlugin(const char* name) noexcept
	{
		if (!gModules || !name)
			return Foxtrot::Status::InvalidArgument;
		try
		{
			if (std::strcmp(name, "Game") == 0)
			{
				const std::wstring gameFile = ResolveGameDll();
				return gModules->Reload(name, gameFile.c_str());
			}
			return gModules->Reload(name);
		}
		catch (...)
		{
			return Foxtrot::Status::Failed;
		}
	}

	/// @brief Loads the requested modules, executes a session, and shuts them down in dependency order.
	/// @param frames Frame count or animation frame descriptions.
	/// @param editor Whether to load and run editor services.
	/// @param failAfterRuntime Whether to exercise cleanup after runtime initialization.
	/// @return Zero on success, or a nonzero failure status.
	/// @throws std::runtime_error If the operation encounters the failure condition checked by this implementation.
	int RunSession(uint32_t frames, bool editor, bool failAfterRuntime)
	{
		ModuleHost modules;
		gModules = &modules;
		struct ClearHost
		{
			~ClearHost() { gModules = nullptr; }
		} clearHost;

		// Loading Core Module.
		modules.Load(L"Core.dll", "Core");

		// Loading EngineRuntime Module.
		const Foxtrot::ModuleAPI* runtime = modules.Load(L"EngineRuntime.dll", "EngineRuntime");
		using SetReloader = void(FT_CALL*)(Foxtrot::Status(FT_CALL*)(const char*) noexcept) noexcept;
		auto setReloader  = reinterpret_cast<SetReloader>(GetProcAddress(GetModuleHandleW(L"EngineRuntime.dll"), "SetPluginReloader"));
		if (!setReloader)
			throw std::runtime_error("Plugin reload entry point missing");
		setReloader(&ReloadHostedPlugin);
		using SetUnloader = void(FT_CALL*)(Foxtrot::Status(FT_CALL*)(const char*) noexcept) noexcept;
		auto setUnloader  = reinterpret_cast<SetUnloader>(GetProcAddress(GetModuleHandleW(L"EngineRuntime.dll"), "SetPluginUnloader"));
		if (!setUnloader)
			throw std::runtime_error("Plugin unload entry point missing");
		setUnloader(&UnloadHostedPlugin);
		Foxtrot::RuntimeControl*  control = static_cast<Foxtrot::RuntimeControl*>(runtime->Query(runtime->Instance, "RuntimeControl"));
		if (!control)
			throw std::runtime_error("Runtime control API missing");

		// RAII pattern to ensure runtime is stopped before ModuleHost shuts down.
		struct StopWorld
		{
			Foxtrot::RuntimeControl* Control;
			~StopWorld() { Control->Stop(); }
		} stop{ control };

		if (failAfterRuntime)
			throw std::runtime_error("Injected startup failure after Runtime");

		auto attach = [&](const wchar_t* file, const char* name) {
			const Foxtrot::ModuleAPI* module = modules.Load(file, name);
			if (control->Attach(name, module->Query(module->Instance, "LegacyPlugin")) != Foxtrot::Status::Ok)
				throw std::runtime_error("Cannot attach plugin to runtime");
		};

		// DLL List stores the plugin id. D3D11's file and module name are FoxtrotD3D11.
		auto resolveModule = [](const Common::FTDS::String& entry, Common::FTDS::String& fileName, Common::FTDS::String& moduleName) {
			if (entry.Equal("D3D11") || entry.Equal("FoxtrotD3D11"))
			{
				fileName.Assign(Common::DLLPath::D3D11);
				moduleName.Assign("FoxtrotD3D11");
				return;
			}
			fileName.Assign(entry);
			fileName.Append(".dll");
			moduleName.Assign(entry);
		};

		Common::FTDS::Array<Common::FTDS::String>* dllList =
			static_cast<Common::FTDS::Array<Common::FTDS::String>*>(runtime->Query(runtime->Instance, "DLLList"));

		// Behaviors are registered in the project copy, not the DLL copied beside the editor.
		const std::wstring gameFile	  = ResolveGameDll();
		bool			   loadedGame = false;
		for (auto iter = dllList->Begin(); iter != dllList->End(); ++iter)
		{
			if ((*iter).IsEmpty())
				continue;
			if ((*iter).Equal("Game"))
			{
				attach(gameFile.c_str(), "Game");
				loadedGame = true;
				continue;
			}

			Common::FTDS::String fileName;
			Common::FTDS::String moduleName;
			resolveModule(*iter, fileName, moduleName);
			const wchar_t* wideFile = fileName.WC_Str();
			attach(wideFile, moduleName.C_Str());
			delete[] wideFile;
		}
		if (!loadedGame)
			attach(gameFile.c_str(), "Game");

		if (editor)
			attach(L"Editor.dll", "Editor");
		return control->Run(frames) == Foxtrot::Status::Ok ? 0 : 1;
	}

	/// @brief Checks that the session's dynamically loaded engine modules have been released.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool ModulesUnloaded()
	{
		for (const auto* name : { L"Core.dll", L"EngineRuntime.dll", L"FoxtrotD3D11.dll", L"Physics2D.dll", L"Game.dll", L"Editor.dll" })
		{
			if (GetModuleHandleW(name))
			{
				std::fprintf(stderr, "A module remained loaded after shutdown\n");
				return false;
			}
		}
		return true;
	}
} // namespace

/// @brief Parses launch options and runs the engine session, reporting failures through the process exit status.
/// @param argc Number of command-line arguments.
/// @param argv Null-terminated command-line argument strings.
/// @return Zero on success, or a nonzero failure status.
int main(int argc, char** argv)
{
	// Maximum number of engine update/render cycles that a session should execute.
	// When 0, no frame limit; run until the application requests exit, such as when the window closes.
	uint32_t frames			  = 0;
	unsigned sessions		  = 1;
	bool	 failAfterRuntime = false;
	bool	 editor			  = false;
#ifdef FOXTROT_EDITOR
	editor = true;
#endif
	for (int i = 1; i < argc; ++i)
	{
		if (std::strcmp(argv[i], "--smoke-test") == 0)
			frames = 3;
		if (std::strcmp(argv[i], "--reload-test") == 0)
		{
			frames	 = 3;
			sessions = 2;
		}
		if (std::strcmp(argv[i], "--fail-after-runtime") == 0)
			failAfterRuntime = true;
		if (std::strcmp(argv[i], "--editor") == 0)
		{
#ifndef FOXTROT_EDITOR
			std::fprintf(stderr, "Use the Foxtrot_Editor_Debug configuration for Editor.\n");
			return 1;
#else
			editor = true;
#endif
		}
	}
	try
	{
		for (unsigned session = 0; session < sessions; ++session)
		{
			int result = RunSession(frames, editor, failAfterRuntime);
			if (!ModulesUnloaded())
				return 2;
			if (result)
				return result;
		}
		return 0;
	}
	catch (const std::exception& e)
	{
		std::fprintf(stderr, "Foxtrot startup failed: %s\n", e.what());
		return ModulesUnloaded() ? 1 : 2;
	}
}
