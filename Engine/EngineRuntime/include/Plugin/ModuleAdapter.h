#pragma once
#include "Foxtrot/Core/CoreApi.h"
#include "Plugin/IPlugin.h"
#include <cstring>
#include <exception>
namespace Foxtrot {
// Internal same-toolchain bridge for the existing component interfaces.
struct PluginAdapter {
    Common::IPlugin* plugin{};
    Common::IPlugin* (*create)();
    bool initialized{};
    static Status FT_CALL Initialize(void* ptr, const HostServices*) noexcept {
        auto& self = *static_cast<PluginAdapter*>(ptr);
        try {
            self.plugin = self.create();
            if (!self.plugin) return Status::Failed;
            self.initialized = true;
            self.plugin->Initialize(); self.plugin->Setup();
            return Status::Ok;
        } catch (const std::exception& e) { FtLog(e.what()); return Status::Failed; }
          catch (...) { FtLog("Plugin initialization failed"); return Status::Failed; }
    }
    static void FT_CALL Shutdown(void* ptr) noexcept {
        auto& self = *static_cast<PluginAdapter*>(ptr);
        if (self.initialized && self.plugin) {
            self.initialized = false;
            try { self.plugin->ShutDown(); } catch (...) { FtLog("Plugin shutdown failed"); }
        }
    }
    static void FT_CALL Destroy(void* ptr) noexcept {
        auto self = static_cast<PluginAdapter*>(ptr);
        Shutdown(ptr); delete self->plugin; delete self;
    }
    static void* FT_CALL Query(void* ptr, const char* name) noexcept {
        auto self = static_cast<PluginAdapter*>(ptr);
        if (!name || !self->plugin) return nullptr;
        if (std::strcmp(name, "LegacyPlugin") == 0) return self->plugin;
        return self->plugin->QueryInterface(name);
    }
};
inline Status MakePluginApi(uint32_t abi, uint32_t bytes, ModuleApi* out, const char* name, Common::IPlugin* (*create)()) noexcept {
    if (!out || abi != ModuleAbi || bytes != sizeof(*out)) return Status::AbiMismatch;
    try {
        auto state = new PluginAdapter{nullptr, create, false};
        *out = {sizeof(*out), ModuleAbi, BuildAbi, sizeof(void*), name, state,
            PluginAdapter::Initialize, PluginAdapter::Shutdown, PluginAdapter::Destroy, PluginAdapter::Query};
        return Status::Ok;
    } catch (...) { return Status::Failed; }
}
}
