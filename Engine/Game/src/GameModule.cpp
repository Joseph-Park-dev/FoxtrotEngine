#include "Foxtrot/Core/CoreApi.h"
#include "Foxtrot/Runtime/GameServices.h"
#include <cstring>
namespace {
struct GameState {
    const Foxtrot::GameServices* services{};
    uint64_t system{};
    uint64_t frames{};
};
void FT_CALL Tick(void* context, float) noexcept {
    auto& game = *static_cast<GameState*>(context);
    if (game.frames++ == 0) FtLog("Game update system running");
    // Game-specific systems and rules belong here; engine objects stay in Runtime.
}
Foxtrot::Status FT_CALL Initialize(void* context, const Foxtrot::HostServices* host) noexcept {
    if (!host || host->size != sizeof(*host) || !host->find) return Foxtrot::Status::InvalidArgument;
    auto& game = *static_cast<GameState*>(context);
    auto runtime = host->find(host->context, "EngineRuntime");
    if (!runtime) return Foxtrot::Status::Failed;
    game.services = static_cast<const Foxtrot::GameServices*>(runtime->query(runtime->instance, "GameServices"));
    if (!game.services || game.services->size != sizeof(Foxtrot::GameServices)) return Foxtrot::Status::AbiMismatch;
    auto status = game.services->registerSystem(Tick, &game, &game.system);
    if (status == Foxtrot::Status::Ok) FtLog("Game initialized");
    return status;
}
void FT_CALL Shutdown(void* context) noexcept {
    auto& game = *static_cast<GameState*>(context);
    if (game.system && game.services) {
        game.services->unregisterSystem(game.system);
        game.system = 0;
        FtLog("Game shutdown");
    }
    game.services = nullptr;
}
void FT_CALL Destroy(void* context) noexcept { Shutdown(context); delete static_cast<GameState*>(context); }
void* FT_CALL Query(void*, const char*) noexcept { return nullptr; }
}
FT_EXPORT Foxtrot::Status FT_CALL FtGetModuleApi(uint32_t abi, uint32_t bytes, Foxtrot::ModuleApi* out) noexcept {
    if (!out || bytes != sizeof(*out) || abi != Foxtrot::ModuleAbi) return Foxtrot::Status::AbiMismatch;
    try {
        auto state = new GameState;
        *out = {sizeof(*out), Foxtrot::ModuleAbi, Foxtrot::BuildAbi, sizeof(void*), "Game", state,
            Initialize, Shutdown, Destroy, Query};
        return Foxtrot::Status::Ok;
    } catch (...) { return Foxtrot::Status::Failed; }
}
