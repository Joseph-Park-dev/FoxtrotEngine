// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// This defines the type of the events enqueued into EventManager 
/// </summary>

#pragma once

enum class EVENT_TYPE
{
	CREATE_ACTOR,
	CREATE_CHILD,
	DESTROY_ACTOR,
	SWITCH_SCENE,
	
	END
};