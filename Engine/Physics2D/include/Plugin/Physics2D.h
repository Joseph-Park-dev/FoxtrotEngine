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

namespace Physics2D
{
	class Physics2D;

	Physics2D* GetPhysics2D();
	b2WorldId& GetPhysics2DWorldID();
} // namespace Physics2D