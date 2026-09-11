#include "Foxtrot/Core/CoreApi.h"
#include "Foxtrot/Runtime/GameServices.h"
#include <cstring>
namespace
{
	struct GameState
	{
		const Foxtrot::GameServices* Services{};
		uint64_t					 System{};
		uint64_t					 Frames{};
	};
	/// @brief Advances the game module's registered system callback for the current frame.
	/// @param context Context associated with this operation.
	/// @note Unnamed parameter (float): reserved by this interface or unused by this implementation.
	void FT_CALL Tick(void* context, float) noexcept
	{
		auto& game = *static_cast<GameState*>(context);
		if (game.Frames++ == 0)
			FtLog("Game update system running");
		// Game-specific systems and rules belong here; engine objects stay in Runtime.
	}
	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param context Context associated with this operation.
	/// @param host Host services available to the module.
	/// @return Status indicating whether the operation completed successfully.
	Foxtrot::Status FT_CALL Initialize(void* context, const Foxtrot::HostServices* host) noexcept
	{
		if (!host || host->Size != sizeof(*host) || !host->Find)
			return Foxtrot::Status::InvalidArgument;
		auto& game	  = *static_cast<GameState*>(context);
		auto  runtime = host->Find(host->Context, "EngineRuntime");
		if (!runtime)
			return Foxtrot::Status::Failed;
		game.Services = static_cast<const Foxtrot::GameServices*>(runtime->Query(runtime->Instance, "GameServices"));
		if (!game.Services || game.Services->Size != sizeof(Foxtrot::GameServices))
			return Foxtrot::Status::AbiMismatch;
		auto status = game.Services->RegisterSystem(Tick, &game, &game.System);
		if (status == Foxtrot::Status::Ok)
			FtLog("Game initialized");
		return status;
	}

	/// @brief Releases initialized services and resources during shutdown.
	/// @param context Context associated with this operation.
	void FT_CALL Shutdown(void* context) noexcept
	{
		auto& game = *static_cast<GameState*>(context);
		if (game.System && game.Services)
		{
			game.Services->UnregisterSystem(game.System);
			game.System = 0;
			FtLog("Game shutdown");
		}
		game.Services = nullptr;
	}

	/// @brief Releases the managed instance or schedules the specified actor for destruction.
	/// @param context Context associated with this operation.
	void FT_CALL Destroy(void* context) noexcept
	{
		Shutdown(context);
		delete static_cast<GameState*>(context);
	}

	/// @brief Returns the named module service supported by this ABI adapter.
	/// @note Unnamed parameter (void*): reserved by this interface or unused by this implementation.
	/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
	/// @return Borrowed interface pointer, or nullptr when the interface is unsupported.
	void* FT_CALL Query(void*, const char*) noexcept
	{
		return nullptr;
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
	try
	{
		auto state = new GameState;
		*out	   = { sizeof(*out), Foxtrot::ModuleAbi, Foxtrot::BuildAbi, sizeof(void*), "Game", state, Initialize, Shutdown, Destroy, Query };
		return Foxtrot::Status::Ok;
	}
	catch (...)
	{
		return Foxtrot::Status::Failed;
	}
}
