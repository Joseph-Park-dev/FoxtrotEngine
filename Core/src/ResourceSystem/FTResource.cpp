// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FTResource.h"

#include <fstream>

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "TemplateFunctions.h"
#include "FTCore.h"

#ifdef FOXTROT_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui/ImGuiFileDialog/ImGuiFileDialog.h>

#include "EditorLayer.h"
#include "DirectoryHelper.h"
#endif //FOXTROT_EDITOR

void FTResource::SaveProperties(std::ofstream& ofs)
{
	// Makes file path relative to the project dir.
	FTDS::String path = *mRelativePath;
	path.ExtractFromLast("\\Assets\\");

	FTDS::String buf(".\\");
	buf.Append(path);

	FileIOHelper::SaveString(ofs, ChunkKey::FTResource::FILE_NAME, *mFileName);
	FileIOHelper::SaveString(ofs, ChunkKey::FTResource::RELATIVE_PATH, buf);
}

// When loading properties, invert the order of the member variables
// (Due to the loading order)
void FTResource::LoadProperties(std::ifstream& ifs)
{
	if (!mRelativePath->IsEmpty())
		return;
	FileIOHelper::LoadBasicString(ifs, *mRelativePath);

	if (!mFileName->IsEmpty())
		return;
	FileIOHelper::LoadBasicString(ifs, *mFileName);

#ifdef FOXTROT_EDITOR
	// Removes the dot in the front.
	ExtractUntil(*mRelativePath, ".");
	mRelativePath->Assign(PATH_PROJECT);
	mRelativePath->Assign(*mRelativePath);
#endif
}

const FTDS::String& FTResource::GetFileName() const
{
	return *mFileName;
}

const FTDS::String& FTResource::GetRelativePath() const
{
	return *mRelativePath;
}

FTResource::FTResource(FTResourceDef& resDef)
	: mFileName(DBG_NEW FTDS::String(resDef.FileName))
	, mRelativePath(DBG_NEW FTDS::String(resDef.RelativePath))
	, mRefCount(0)
	, mIsProcessed(false)
{
}

FTResource::~FTResource()
{
	delete mFileName;
	delete mRelativePath;
	mRefCount = 0;
	mIsProcessed = false;
}

void FTResource::Process()
{
	mIsProcessed = true;
}

void FTResource::Process(FoxtrotRenderer* renderer)
{
	mIsProcessed = true;
}

const bool FTResource::IsProcessed() const
{
	return mIsProcessed;
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

FTResourceDef::FTResourceDef()
	: FileName()
	, RelativePath()
{
}

FTResourceDef::FTResourceDef(const char* fileName, const char* relPath)
	: FileName(fileName)
	, RelativePath(relPath)
{
}

FTResourceDef::FTResourceDef(const FTDS::String& fileName, const FTDS::String& relPath)
	: FileName(fileName.C_Str())
	, RelativePath(relPath.C_Str())
{
}