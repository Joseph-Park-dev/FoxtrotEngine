// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component.h"

#include "Plugin/IPlugin.h"
#include "Actor/Actor.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "TemplateFunctions.h"
#include "FTCore.h"
#include "Debugging/DebugMemAlloc.h"

#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
	#include "EditorHelper.h"
#endif // FOXTROT_EDITOR

namespace Core
{
	using namespace Editor;
	const bool Component::GetIsSetup() const
	{
		return mIsSetup;
	}

	const bool Component::GetIsActive() const
	{
		return mIsActive;
	}

	void Component::SetIsActive(bool isActive)
	{
		mIsActive = isActive;
	}

#ifdef FOXTROT_EDITOR
	void Component::EditorUIUpdate()
	{
		Editor::UPDATE_BOOL("Is Active", mIsActive);
	}

	void Component::SetUpdateOrder(int updateOrder)
	{
		mUpdateOrder = updateOrder;
	}
#endif
} // namespace Core