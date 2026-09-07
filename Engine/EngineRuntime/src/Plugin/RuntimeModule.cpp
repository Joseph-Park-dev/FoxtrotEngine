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

namespace {
struct System { uint64_t token; Foxtrot::SystemTick tick; void* context; };
std::vector<System> systems;
uint64_t nextToken = 1;
bool ticking = false;
Foxtrot::Status FT_CALL RegisterSystem(Foxtrot::SystemTick tick, void* context, uint64_t* token) noexcept {
    if (!tick || !token) return Foxtrot::Status::InvalidArgument;
    if (ticking) return Foxtrot::Status::Busy;
    try { systems.push_back({nextToken, tick, context}); *token = nextToken++; return Foxtrot::Status::Ok; }
    catch (...) { return Foxtrot::Status::Failed; }
}
Foxtrot::Status FT_CALL UnregisterSystem(uint64_t token) noexcept {
    if (ticking) return Foxtrot::Status::Busy;
    auto it = std::find_if(systems.begin(), systems.end(), [token](const auto& s) { return s.token == token; });
    if (it == systems.end()) return Foxtrot::Status::InvalidArgument;
    systems.erase(it); return Foxtrot::Status::Ok;
}
void FT_CALL RequestExit() noexcept { Core::FTCore::GetInstance()->SetIsRunning(false); }
Foxtrot::GameServices gameServices{sizeof(Foxtrot::GameServices), RegisterSystem, UnregisterSystem, RequestExit};
bool comInitialized = false;
bool initialized = false;
bool stopped = false;
Foxtrot::Status FT_CALL Initialize(void*, const Foxtrot::HostServices*) noexcept {
    try {
        initialized = true; stopped = false;
        const auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr)) return Foxtrot::Status::Failed;
        comInitialized = true;
        if (!Core::FTCore::GetInstance()->Initialize()) return Foxtrot::Status::Failed;
        FtLog("EngineRuntime initialized");
        return Foxtrot::Status::Ok;
    } catch (const std::exception& e) { FtLog(e.what()); return Foxtrot::Status::Failed; }
      catch (...) { return Foxtrot::Status::Failed; }
}
void FT_CALL Stop() noexcept {
    if (!initialized || stopped) return;
    stopped = true;
    try {
        auto scene = Core::SceneManager::GetInstance()->GetCurrentScene();
        if (scene) scene->DeleteAll();
        Core::ResourceManager::GetInstance()->ReleaseWorldResources();
        Core::FTCore::GetInstance()->SetIsRunning(false);
    } catch (...) { FtLog("World shutdown failed"); }
}
void FT_CALL Shutdown(void*) noexcept {
    if (!initialized) return;
    Stop();
    try { Core::FTCore::GetInstance()->ShutDown(); } catch (...) { FtLog("Runtime shutdown failed"); }
    systems.clear();
    if (comInitialized) { CoUninitialize(); comInitialized = false; }
    initialized = false;
}
void FT_CALL Destroy(void*) noexcept { Shutdown(nullptr); Core::FTCore::Destroy(); }
Foxtrot::Status FT_CALL Attach(const char* name, void* plugin) noexcept {
    if (!name || !plugin) return Foxtrot::Status::InvalidArgument;
    try { Core::PluginManager::GetInstance()->Attach(name, static_cast<Common::IPlugin*>(plugin)); return Foxtrot::Status::Ok; }
    catch (...) { return Foxtrot::Status::Failed; }
}
Foxtrot::Status FT_CALL Run(uint32_t maxFrames) noexcept {
    try { Core::FTCore::GetInstance()->RunFrames(maxFrames); return Foxtrot::Status::Ok; }
    catch (const std::exception& e) { FtLog(e.what()); return Foxtrot::Status::Failed; }
    catch (...) { FtLog("Frame failed"); return Foxtrot::Status::Failed; }
}
Foxtrot::RuntimeControl control{Attach, Run, Stop};
void* FT_CALL Query(void*, const char* name) noexcept {
    if (!name) return nullptr;
    if (std::strcmp(name, "GameServices") == 0) return &gameServices;
    return std::strcmp(name, "RuntimeControl") == 0 ? &control : nullptr;
}
}
FT_EXPORT Foxtrot::Status FT_CALL FtGetModuleApi(uint32_t abi, uint32_t bytes, Foxtrot::ModuleApi* out) noexcept {
    if (!out || bytes != sizeof(*out) || abi != Foxtrot::ModuleAbi) return Foxtrot::Status::AbiMismatch;
    *out = {sizeof(*out), Foxtrot::ModuleAbi, Foxtrot::BuildAbi, sizeof(void*), "EngineRuntime", nullptr,
        Initialize, Shutdown, Destroy, Query};
    return Foxtrot::Status::Ok;
}

void Foxtrot::UpdateGameSystems(float dt) noexcept {
    ticking = true;
    for (const auto& system : systems) system.tick(system.context, dt);
    ticking = false;
}
