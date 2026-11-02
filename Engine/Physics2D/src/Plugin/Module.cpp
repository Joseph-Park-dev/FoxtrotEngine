// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "pch.h"
#include "Plugin/Physics2D.h"
#include "Plugin/ModuleAdapter.h"
#include "Debugging/DebugMemAlloc.h"

extern "C" Common::IPlugin* CreatePlugin(const char*);

/// @brief Constructs the plugin through the exported factory.
/// @return Created plugin instance.
static Common::IPlugin* Create()
{
	return CreatePlugin("Physics2D");
}

/// @brief Validates the requested ABI and publishes this module's lifecycle descriptor.
/// @param abi ABI version requested by the host.
/// @param bytes Size of the ABI output buffer in bytes.
/// @param out Receives the operation's output.
/// @return ABI status indicating success or why the descriptor could not be produced.
FT_EXPORT Foxtrot::Status FT_CALL FtGetModuleAPI(uint32_t abi, uint32_t bytes, Foxtrot::ModuleAPI* out) noexcept
{
	return Foxtrot::MakePluginAPI(abi, bytes, out, "Physics2D", Create);
}
