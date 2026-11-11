// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Plugin/CommandHistoryAccess.h"

#ifdef FOXTROT_EDITOR
	#include "Plugin/GetFunc.h"
	#include "FileSystem/DLLPath.h"

namespace Editor
{
	static ICommandHistory* sCommandHistory = nullptr;

	/// @brief Resolves GetCMDHistory from Editor.dll. Safe to call more than once.
	void AcquireCommandHistory()
	{
		if (sCommandHistory != nullptr)
			return;

		// D3D11/include/Plugin/PluginKey.h hides EngineRuntime's ProcNames.
		sCommandHistory = Core::GetFunc<GET_CMD_HISTORY>(
			Common::DLLPath::EDITOR,
			"GetCMDHistory")();
	}

	/// @brief Returns the command history stored by AcquireCommandHistory.
	/// @return Borrowed command history. Null until AcquireCommandHistory has run.
	ICommandHistory* GetCommandHistory()
	{
		if (!sCommandHistory)
			AcquireCommandHistory();
		return sCommandHistory;
	}
} // namespace Editor
#endif // FOXTROT_EDITOR
