#include "ModuleHost.h"
#include "Foxtrot/Runtime/RuntimeApi.h"
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace
{
	/// @brief Loads the requested modules, executes a session, and shuts them down in dependency order.
	/// @param frames Frame count or animation frame descriptions.
	/// @param editor Whether to load and run editor services.
	/// @param failAfterRuntime Whether to exercise cleanup after runtime initialization.
	/// @return Zero on success, or a nonzero failure status.
	/// @throws std::runtime_error If the operation encounters the failure condition checked by this implementation.
	int RunSession(uint32_t frames, bool editor, bool failAfterRuntime)
	{
		ModuleHost modules;

		// Loading Core Module.
		modules.Load(L"Core.dll", "Core");

		// Loading EngineRuntime Module.
		auto runtime = modules.Load(L"EngineRuntime.dll", "EngineRuntime");
		auto control = static_cast<Foxtrot::RuntimeControl*>(runtime->Query(runtime->Instance, "RuntimeControl"));
		if (!control)
			throw std::runtime_error("Runtime control API missing");

		struct StopWorld
		{
			Foxtrot::RuntimeControl* Control;
			~StopWorld() { Control->Stop(); }
		} stop{ control };
		if (failAfterRuntime)
			throw std::runtime_error("Injected startup failure after Runtime");
		auto attach = [&](const wchar_t* file, const char* name) {
			auto module = modules.Load(file, name);
			if (control->Attach(name, module->Query(module->Instance, "LegacyPlugin")) != Foxtrot::Status::Ok)
				throw std::runtime_error("Cannot attach plugin to runtime");
		};
		attach(L"FoxtrotD3D11.dll", "D3D11");
		modules.Load(L"Game.dll", "Game");
		if (editor)
			attach(L"Editor.dll", "Editor");
		return control->Run(frames) == Foxtrot::Status::Ok ? 0 : 1;
	}
	/// @brief Checks that the session's dynamically loaded engine modules have been released.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool ModulesUnloaded()
	{
		for (const auto* name : { L"Core.dll", L"EngineRuntime.dll", L"FoxtrotD3D11.dll", L"Game.dll", L"Editor.dll" })
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
