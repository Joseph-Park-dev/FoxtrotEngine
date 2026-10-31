// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Function-pointer factories that construct a resource from its file name, path, and renderer.
/// </summary>
#pragma once
#include "ResourceSystem/FTResource.h"

using ResourceFactoryEntry = Common::IResource* (*)(Common::FTResourceDef& resDef, void* renderer);

/// @brief Constructs a resource whose constructor is (FTResourceDef&, void*).
/// @param resDef Resource definition containing the filename and source path.
/// @param renderer Renderer forwarded to graphics resources. Core resources receive nullptr.
/// @return Created resource, owned by the caller.
template <typename FTRESOURCE>
Common::IResource* CreateRegisteredResource(Common::FTResourceDef& resDef, void* renderer)
{
	return DBG_NEW FTRESOURCE(resDef, renderer);
}
