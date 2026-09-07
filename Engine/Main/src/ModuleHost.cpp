#include "ModuleHost.h"
#include <cstring>
#include <cstdio>
#include <stdexcept>

ModuleHost::ModuleHost()
{
    wchar_t path[32768]{};
    DWORD count = GetModuleFileNameW(nullptr, path, 32768);
    if (!count || count == 32768) throw std::runtime_error("Cannot resolve executable directory");
    directory.assign(path, count);
    directory.resize(directory.find_last_of(L"\\/") + 1);
    services = {sizeof(services), this, [](void* self, const char* name) noexcept {
        return static_cast<ModuleHost*>(self)->Find(name);
    }};
}
ModuleHost::~ModuleHost() { Shutdown(); }
const Foxtrot::ModuleApi* ModuleHost::Find(const char* name) const noexcept
{
    if (!name) return nullptr;
    for (const auto& e : entries)
        if (e->api.name && std::strcmp(name, e->api.name) == 0) return &e->api;
    return nullptr;
}
const Foxtrot::ModuleApi* ModuleHost::Load(const wchar_t* file, const char* expectedName)
{
    if (auto existing = Find(expectedName)) return existing;
    auto e = std::make_unique<Entry>();
    const auto path = directory + file;
    e->handle = LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (!e->handle) throw std::runtime_error(std::string("Cannot load ") + expectedName + " (Win32 " + std::to_string(GetLastError()) + ")");
    try {
        auto get = reinterpret_cast<Foxtrot::GetModuleApi>(GetProcAddress(e->handle, "FtGetModuleApi"));
        if (!get || get(Foxtrot::ModuleAbi, sizeof(e->api), &e->api) != Foxtrot::Status::Ok)
            throw std::runtime_error("Missing or incompatible module entry point");
        if (e->api.size != sizeof(e->api) || e->api.abi != Foxtrot::ModuleAbi ||
            e->api.build != Foxtrot::BuildAbi || e->api.pointerBytes != sizeof(void*) ||
            !e->api.name || std::strcmp(e->api.name, expectedName) != 0 ||
            !e->api.initialize || !e->api.shutdown || !e->api.destroy || !e->api.query)
            throw std::runtime_error("Invalid module API descriptor");
        e->initialized = true; // Shutdown must tolerate partial initialization.
        if (e->api.initialize(e->api.instance, &services) != Foxtrot::Status::Ok)
            throw std::runtime_error(std::string("Initialization failed: ") + expectedName);
        std::fprintf(stderr, "Loaded %s\n", expectedName);
        entries.push_back(std::move(e));
        return &entries.back()->api;
    } catch (...) {
        if (e->initialized) e->api.shutdown(e->api.instance);
        if (e->api.destroy) e->api.destroy(e->api.instance);
        FreeLibrary(e->handle);
        throw;
    }
}
void ModuleHost::Shutdown() noexcept
{
    while (!entries.empty()) {
        auto e = std::move(entries.back());
        entries.pop_back();
        if (e->initialized) e->api.shutdown(e->api.instance);
        e->api.destroy(e->api.instance);
        std::fprintf(stderr, "Unloading %s\n", e->api.name);
        FreeLibrary(e->handle);
    }
}
