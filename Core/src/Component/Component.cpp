// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Component.h"

#include "Plugin/Plugin.h"
#include "Actor/Actor.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "TemplateFunctions.h"
#include "FTCore.h"
#include "Debugging/DebugMemAlloc.h"

namespace Core
{
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
		CommandHistory::GetInstance()->UpdateBoolValue("Is Active", mIsActive);
	}

	void Component::SetUpdateOrder(int updateOrder)
	{
		mUpdateOrder = updateOrder;
	}
#endif
} // namespace Core