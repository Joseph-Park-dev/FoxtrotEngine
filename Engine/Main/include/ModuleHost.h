#pragma once
#include "Foxtrot/Core/ModuleApi.h"
#include <Windows.h>
#include <memory>
#include <string>
#include <vector>

// Bootstrap code lives in the EXE, so it can unload the last DLL safely.
class ModuleHost
{
public:
    ModuleHost();
    ~ModuleHost();
    ModuleHost(const ModuleHost&) = delete;
    ModuleHost& operator=(const ModuleHost&) = delete;
    const Foxtrot::ModuleApi* Load(const wchar_t* file, const char* expectedName);
    const Foxtrot::ModuleApi* Find(const char* name) const noexcept;
    void Shutdown() noexcept;
private:
    struct Entry { HMODULE handle{}; Foxtrot::ModuleApi api{}; bool initialized{}; };
    std::wstring directory;
    std::vector<std::unique_ptr<Entry>> entries;
    Foxtrot::HostServices services{};
};
