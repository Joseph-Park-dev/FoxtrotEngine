// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A wrapper class for arbitary files holding texts.
/// </summary>

#pragma once
#include "ResourceSystem/FTResource.h"

class FTText :
	public FTResource
{
public:
	virtual void Process(FTCore* coreInst) override;
};
