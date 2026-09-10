#include "Manager/ResourceManager.h"
#include "Foxtrot/Runtime/GameServices.h"
#include "Plugin/GameSystems.h"
#include <vector>
#include <algorithm>
#include "Foxtrot/Core/CoreApi.h"
#include "Foxtrot/Runtime/RuntimeApi.h"
#include "Core/FTCore.h"
#include "Manager/PluginManager.h"
#include "Manager/SceneManager.h"
#include "Scene/Scene.h"
#include <cstring>
#include <exception>

namespace
{
	struct System
	{
		uint64_t			Token;
		Foxtrot::SystemTick Tick;
		void*				Context;
	};

	std::vector<System> systems;
	uint64_t			nextToken = 1;
	bool				ticking	  = false;

	/// @brief Registers a callback and context for game-system updates.
	/// @param tick Callback invoked once per game-system update.
	/// @param context Context associated with this operation.
	/// @param token Registration token identifying a game system.
	/// @return Status indicating whether the operation completed successfully.
	/// @note Main-thread-only; unregister callbacks before unloading their owning module.
	Foxtrot::Status FT_CALL RegisterSystem(Foxtrot::SystemTick tick, void* context, uint64_t* token) noexcept
	{
		if (!tick || !token)
			return Foxtrot::Status::InvalidArgument;
		if (ticking)
			return Foxtrot::Status::Busy;
		try
		{
			systems.push_back({ nextToken, tick, context });
			*token = nextToken++;
			return Foxtrot::Status::Ok;
		}
		catch (...)
		{
			return Foxtrot::Status::Failed;
		}
	}

	/// @brief Removes a previously registered game-system callback.
	/// @param token Registration token identifying a game system.
	/// @return Status indicating whether the operation completed successfully.
	/// @note Main-thread-only; unregister callbacks before unloading their owning module.
	Foxtrot::Status FT_CALL UnregisterSystem(uint64_t token) noexcept
	{
		if (ticking)
			return Foxtrot::Status::Busy;
		auto it = std::find_if(systems.begin(), systems.end(), [token](const auto& s) { return s.Token == token; });
		if (it == systems.end())
			return Foxtrot::Status::InvalidArgument;
		systems.erase(it);
		return Foxtrot::Status::Ok;
	}

	/// @brief Requests termination of the active runtime loop.
	void FT_CALL		  RequestExit() noexcept { Core::FTCore::GetInstance()->SetIsRunning(false); }
	Foxtrot::GameServices gameServices{ sizeof(Foxtrot::GameServices), RegisterSystem, UnregisterSystem, RequestExit };
	bool				  comInitialized = false;
	bool				  initialized	 = false;
	bool				  stopped		 = false;

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @note Unnamed parameter (void*): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (const Foxtrot::HostServices*): reserved by this interface or unused by this implementation.
	/// @return Status indicating whether the operation completed successfully.
	Foxtrot::Status FT_CALL Initialize(void*, const Foxtrot::HostServices*) noexcept
	{
		try
		{
			initialized	  = true;
			stopped		  = false;
			const auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			if (FAILED(hr))
				return Foxtrot::Status::Failed;
			comInitialized = true;
			if (!Core::FTCore::GetInstance()->Initialize())
				return Foxtrot::Status::Failed;
			FtLog("EngineRuntime initialized");
			return Foxtrot::Status::Ok;
		}
		catch (const std::exception& e)
		{
			FtLog(e.what());
			return Foxtrot::Status::Failed;
		}
		catch (...)
		{
			return Foxtrot::Status::Failed;
		}
	}

	/// @brief Stops animation playback.
	void FT_CALL Stop() noexcept
	{
		if (!initialized || stopped)
			return;
		stopped = true;
		try
		{
			auto scene = Core::SceneManager::GetInstance()->GetCurrentScene();
			if (scene)
				scene->DeleteAll();
			Core::ResourceManager::GetInstance()->ReleaseWorldResources();
			Core::FTCore::GetInstance()->SetIsRunning(false);
		}
		catch (...)
		{
			FtLog("World shutdown failed");
		}
	}

	/// @brief Releases initialized services and resources during shutdown.
	/// @note Unnamed parameter (void*): reserved by this interface or unused by this implementation.
	void FT_CALL Shutdown(void*) noexcept
	{
		if (!initialized)
			return;
		Stop();
		try
		{
			Core::FTCore::GetInstance()->ShutDown();
		}
		catch (...)
		{
			FtLog("Runtime shutdown failed");
		}
		systems.clear();
		if (comInitialized)
		{
			CoUninitialize();
			comInitialized = false;
		}
		initialized = false;
	}

	/// @brief Releases the managed instance or schedules the specified actor for destruction.
	/// @note Unnamed parameter (void*): reserved by this interface or unused by this implementation.
	void FT_CALL Destroy(void*) noexcept
	{
		Shutdown(nullptr);
		Core::FTCore::Destroy();
	}

	/// @brief Associates the supplied implementation with the owning host or registry.
	/// @param name Name used to identify the requested object or interface.
	/// @param plugin Plugin supplying component or lifecycle services.
	/// @return Status indicating whether the operation completed successfully.
	Foxtrot::Status FT_CALL Attach(const char* name, void* plugin) noexcept
	{
		if (!name || !plugin)
			return Foxtrot::Status::InvalidArgument;
		try
		{
			Core::PluginManager::GetInstance()->Attach(name, static_cast<Common::IPlugin*>(plugin));
			return Foxtrot::Status::Ok;
		}
		catch (...)
		{
			return Foxtrot::Status::Failed;
		}
	}

	/// @brief Executes the requested runtime session.
	/// @param maxFrames Maximum number of frames to run.
	/// @return Status indicating whether the operation completed successfully.
	Foxtrot::Status FT_CALL Run(uint32_t maxFrames) noexcept
	{
		try
		{
			Core::FTCore::GetInstance()->RunFrames(maxFrames);
			return Foxtrot::Status::Ok;
		}
		catch (const std::exception& e)
		{
			FtLog(e.what());
			return Foxtrot::Status::Failed;
		}
		catch (...)
		{
			FtLog("Frame failed");
			return Foxtrot::Status::Failed;
		}
	}

	Foxtrot::RuntimeControl control{ Attach, Run, Stop };
	/// @brief Returns the named module service supported by this ABI adapter.
	/// @note Unnamed parameter (void*): reserved by this interface or unused by this implementation.
	/// @param name Name used to identify the requested object or interface.
	/// @return Borrowed interface pointer, or nullptr when the interface is unsupported.
	void* FT_CALL Query(void*, const char* name) noexcept
	{
		if (!name)
			return nullptr;
		if (std::strcmp(name, "GameServices") == 0)
			return &gameServices;
		return std::strcmp(name, "RuntimeControl") == 0 ? &control : nullptr;
	}
} // namespace

/// @brief Validates the requested ABI and publishes this module's lifecycle and service descriptor.
/// @param abi abi used by this operation.
/// @param bytes Size of the allocation or ABI output buffer in bytes.
/// @param out Receives the operation's output.
/// @return ABI status indicating success or the reason the descriptor could not be produced.
FT_EXPORT Foxtrot::Status FT_CALL FtGetModuleAPI(uint32_t abi, uint32_t bytes, Foxtrot::ModuleAPI* out) noexcept
{
	if (!out || bytes != sizeof(*out) || abi != Foxtrot::ModuleAbi)
		return Foxtrot::Status::AbiMismatch;
	*out = { sizeof(*out), Foxtrot::ModuleAbi, Foxtrot::BuildAbi, sizeof(void*), "EngineRuntime", nullptr, Initialize, Shutdown, Destroy, Query };
	return Foxtrot::Status::Ok;
}

/// @brief Dispatches the frame time step to registered game systems.
/// @param dt Elapsed frame time in seconds.
/// @note Main-thread-only; unregister callbacks before unloading their owning module.
void Foxtrot::UpdateGameSystems(float dt) noexcept
{
	ticking = true;
	for (const auto& system : systems)
		system.Tick(system.Context, dt);
	ticking = false;
}
