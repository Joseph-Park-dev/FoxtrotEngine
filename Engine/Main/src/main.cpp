#include "ModuleHost.h"
#include "Foxtrot/Runtime/RuntimeApi.h"
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace {
int RunSession(uint32_t frames, bool editor, bool failAfterRuntime) {
    ModuleHost modules;
    modules.Load(L"Core.dll", "Core");
    auto runtime = modules.Load(L"EngineRuntime.dll", "EngineRuntime");
    auto control = static_cast<Foxtrot::RuntimeControl*>(runtime->query(runtime->instance, "RuntimeControl"));
    if (!control) throw std::runtime_error("Runtime control API missing");
    struct StopWorld { Foxtrot::RuntimeControl* control; ~StopWorld() { control->stop(); } } stop{control};
    if (failAfterRuntime) throw std::runtime_error("Injected startup failure after Runtime");
    auto attach = [&](const wchar_t* file, const char* name) {
        auto module = modules.Load(file, name);
        if (control->attach(name, module->query(module->instance, "LegacyPlugin")) != Foxtrot::Status::Ok)
            throw std::runtime_error("Cannot attach plugin to runtime");
    };
    attach(L"FoxtrotD3D11.dll", "D3D11");
    modules.Load(L"Game.dll", "Game");
    if (editor) attach(L"Editor.dll", "Editor");
    return control->run(frames) == Foxtrot::Status::Ok ? 0 : 1;
}
bool ModulesUnloaded() {
    for (const auto* name : {L"Core.dll", L"EngineRuntime.dll", L"FoxtrotD3D11.dll", L"Game.dll", L"Editor.dll"}) {
        if (GetModuleHandleW(name)) { std::fprintf(stderr, "A module remained loaded after shutdown\n"); return false; }
    }
    return true;
}
}
int main(int argc, char** argv) {
    uint32_t frames = 0;
    unsigned sessions = 1;
    bool failAfterRuntime = false;
    bool editor = false;
#ifdef FOXTROT_EDITOR
    editor = true;
#endif
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--smoke-test") == 0) frames = 3;
        if (std::strcmp(argv[i], "--reload-test") == 0) { frames = 3; sessions = 2; }
        if (std::strcmp(argv[i], "--fail-after-runtime") == 0) failAfterRuntime = true;
        if (std::strcmp(argv[i], "--editor") == 0) {
#ifndef FOXTROT_EDITOR
            std::fprintf(stderr, "Use the Foxtrot_Editor_Debug configuration for Editor.\n");
            return 1;
#else
            editor = true;
#endif
        }
    }
    try {
        for (unsigned session = 0; session < sessions; ++session) {
            int result = RunSession(frames, editor, failAfterRuntime);
            if (!ModulesUnloaded()) return 2;
            if (result) return result;
        }
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Foxtrot startup failed: %s\n", e.what());
        return ModulesUnloaded() ? 1 : 2;
    }
}
