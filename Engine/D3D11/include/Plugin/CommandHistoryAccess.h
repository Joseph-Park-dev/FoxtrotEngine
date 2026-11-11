// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Resolves Editor.dll's command history for D3D11 editor controls.
/// </summary>
#pragma once

#ifdef FOXTROT_EDITOR
	#include "Plugin/ICommandHistory.h"

namespace Editor
{
	/// @brief Resolves GetCMDHistory from Editor.dll. Safe to call more than once.
	void AcquireCommandHistory();

	/// @brief Returns the command history stored by AcquireCommandHistory.
	/// @return Borrowed command history. Null until AcquireCommandHistory has run.
	ICommandHistory* GetCommandHistory();
} // namespace Editor
#endif // FOXTROT_EDITOR
