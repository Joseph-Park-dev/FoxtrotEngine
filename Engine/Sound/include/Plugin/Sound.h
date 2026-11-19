// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Sound plugin loaded by the engine module host.
/// </summary>
#pragma once

namespace SoLoud
{
	class Soloud;
}

namespace Common
{
	class IComponent;
}

#ifdef FOXTROT_EDITOR
namespace Editor
{
	class ICommandHistory;

	/// @brief Resolves GetCMDHistory from Editor.dll. Safe to call more than once.
	void AcquireCommandHistory();

	/// @brief Returns the command history stored by AcquireCommandHistory.
	/// @return Borrowed command history. Null until AcquireCommandHistory has run.
	ICommandHistory* GetCommandHistory();
} // namespace Editor
#endif

namespace Sound
{
	/// @brief Returns the SoLoud engine owned by the Sound plugin.
	/// @return Borrowed engine. Null until the plugin has initialized playback.
	SoLoud::Soloud* GetSoloud();

	/// @brief Registers a component for plugin-managed lifecycle processing.
	void RegisterOwnedComponent(Common::IComponent* comp);
	/// @brief Removes a component from plugin-managed lifecycle processing.
	void UnregisterOwnedComponent(Common::IComponent* comp);
} // namespace Sound
