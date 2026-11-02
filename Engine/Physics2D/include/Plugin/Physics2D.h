// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Physics2D plugin loaded by the engine module host.
/// </summary>
#pragma once

struct b2WorldId;

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

namespace Physics2D
{
	class Physics2D;

	Physics2D* GetPhysics2D();
	b2WorldId& GetPhysics2DWorldID();

	/// @brief Registers a component for plugin-managed lifecycle processing.
	void RegisterOwnedComponent(Common::IComponent* comp);
	/// @brief Removes a component from plugin-managed lifecycle processing.
	void UnregisterOwnedComponent(Common::IComponent* comp);
} // namespace Physics2D