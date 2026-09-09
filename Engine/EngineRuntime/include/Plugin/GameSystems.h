#pragma once
namespace Foxtrot { /** @brief Dispatches the frame time step to registered game systems.
 * @param deltaSeconds Elapsed frame time in seconds.
 * @note Main-thread-only; unregister callbacks before unloading their owning module. */ void UpdateGameSystems(float deltaSeconds) noexcept; }
