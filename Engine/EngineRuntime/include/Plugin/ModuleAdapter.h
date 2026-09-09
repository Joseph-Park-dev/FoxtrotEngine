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
    /// @brief Initializes the services and state required before this object's runtime lifecycle begins.
    /// @param ptr Opaque module instance supplied by the host.
    /// @note Unnamed parameter (const HostServices*): reserved by this interface or unused by this implementation.
    /// @return Status indicating whether the operation completed successfully.
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
    /// @brief Releases initialized services and resources during shutdown.
    /// @param ptr Opaque module instance supplied by the host.
    static void FT_CALL Shutdown(void* ptr) noexcept {
        auto& self = *static_cast<PluginAdapter*>(ptr);
        if (self.initialized && self.plugin) {
            self.initialized = false;
            try { self.plugin->ShutDown(); } catch (...) { FtLog("Plugin shutdown failed"); }
        }
    }
    /// @brief Releases the managed instance or schedules the specified actor for destruction.
    /// @param ptr Opaque module instance supplied by the host.
    static void FT_CALL Destroy(void* ptr) noexcept {
        auto self = static_cast<PluginAdapter*>(ptr);
        Shutdown(ptr); delete self->plugin; delete self;
    }
    /// @brief Returns the named module service supported by this ABI adapter.
    /// @param ptr Opaque module instance supplied by the host.
    /// @param name Name used to identify the requested object or interface.
    /// @return Borrowed interface pointer, or nullptr when the interface is unsupported.
    static void* FT_CALL Query(void* ptr, const char* name) noexcept {
        auto self = static_cast<PluginAdapter*>(ptr);
        if (!name || !self->plugin) return nullptr;
        if (std::strcmp(name, "LegacyPlugin") == 0) return self->plugin;
        return self->plugin->QueryInterface(name);
    }
};
/// @brief Validates ABI compatibility and constructs the exported plugin lifecycle descriptor.
/// @param abi abi used by this operation.
/// @param bytes Size of the allocation or ABI output buffer in bytes.
/// @param out Receives the operation's output.
/// @param name Name used to identify the requested object or interface.
/// @param create Factory callback that constructs the plugin.
/// @return ABI status indicating success or the reason the descriptor could not be produced.
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
