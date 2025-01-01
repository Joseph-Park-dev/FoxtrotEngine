// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Definition of "new" keyword for debugging purpose to detect memory leak.
/// </summary>

#pragma once
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define DBG_NEW new( _NORMAL_BLOCK , __FILE__ , __LINE__ )

//#ifdef FOXTROT_EDITOR
//#define DBG_NEW new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//#else
//#define DBG_NEW new
//#endif // DEBUG