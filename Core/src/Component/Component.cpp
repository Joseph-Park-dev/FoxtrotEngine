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
	Actor* Component::GetOwner() const
	{
		return mOwner;
	}

	const int Component::GetUpdateOrder() const
	{
		return mUpdateOrder;
	}

	const bool Component::GetIsInitialized() const
	{
		return mIsInitialized;
	}

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

	void Component::Copy(const Component* origin)
	{
		mPlugin		   = origin->mPlugin;
		mOwner		   = origin->mOwner;
		mUpdateOrder   = origin->mUpdateOrder;
		mIsInitialized = false;
		mIsSetup	   = false;
		mIsActive	   = false;

		mOwner->AddComponent(this);
	}

	Plugin* Component::GetPlugin()
	{
		return mPlugin;
	}

	void Component::Copy(Component* to)
	{
		to->mOwner		 = this->mOwner;
		to->mUpdateOrder = this->mUpdateOrder;
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