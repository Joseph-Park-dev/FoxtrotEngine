// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FTResource.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui/imgui.h>
	#include <imgui/backends/imgui_impl_dx11.h>
	#include <imgui/backends/imgui_impl_win32.h>
	#include <imgui/ImGuiFileDialog/ImGuiFileDialog.h>

	#include "EditorLayer.h"
	#include "DirectoryHelper.h"
#endif // FOXTROT_EDITOR

namespace Core
{
	FTResource::FTResource(FTResourceDef& resDef)
	{
		mFileName->Assign(resDef.FileName);
		mRelativePath->Assign(resDef.Path);
	}

#ifdef FOXTROT_EDITOR
	bool FTResource::IsReferenced() const
	{
		return 0 < mRefCount;
	}

	void FTResource::AddRefCount()
	{
		++mRefCount;
	}

	void FTResource::SubtractRefCount()
	{
		--mRefCount;
	}
#endif // FOXTROT_EDITOR
} // namespace Core