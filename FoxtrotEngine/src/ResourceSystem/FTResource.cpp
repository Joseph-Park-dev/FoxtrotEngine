// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/FTResource.h"

#include <fstream>

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Core/TemplateFunctions.h"
#include "Core/FTCore.h"

#ifdef FOXTROT_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include <imgui/ImGuiFileDialog/ImGuiFileDialog.h>

#include "EditorLayer.h"
#include "DirectoryHelper.h"
#endif //FOXTROT_EDITOR

FTResource::FTResource()
    : mFileName{}
    , mRelativePath{}
    , mRefCount(0)
    , mIsProcessed(false)
{}

void FTResource::SaveProperties(std::ofstream& ofs)
{
    // Makes file path relative to the project dir.
    FTDS::String path = mRelativePath;
    path.ExtractFromLast("\\Assets\\");

    FTDS::String buf (".\\");
    buf.Append(path);

    FileIOHelper::SaveString(ofs, ChunkKey::FILE_NAME, mFileName);
    FileIOHelper::SaveString(ofs, ChunkKey::RELATIVE_PATH, buf);
}

// When loading properties, invert the order of the member variables
// (Due to the loading order)
void FTResource::LoadProperties(std::ifstream& ifs)
{
    if (!mRelativePath.IsEmpty())
        return;
    FileIOHelper::LoadBasicString(ifs, mRelativePath);

    if (!mFileName.IsEmpty())
        return;
    FileIOHelper::LoadBasicString(ifs, mFileName);

#ifdef FOXTROT_EDITOR
    // Removes the dot in the front.
    ExtractUntil(mRelativePath, ".");
    mRelativePath = PATH_PROJECT + mRelativePath;
#endif
}

#ifdef FOXTROT_EDITOR
void FTResource::UpdateNameAndPath(FTDS::String fileExtension)
{
    FTDS::String currentPath = "No path has been assigned";
    mRelativePath.Assign(currentPath);
    if (!mRelativePath.IsEmpty())
    {
        currentPath.Assign("Current path : \n");
        currentPath.Append(mRelativePath);
    }

    if (ImGui::Button("Select File")) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.countSelectionMax = 1;
        ImGuiFileDialog::Instance()->OpenDialog("SelectFile", "Select File", fileExtension.C_Str(), config);
        ImGui::OpenPopup("Select File");
    }

    if (ImGuiFileDialog::Instance()->Display("SelectFile"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            mRelativePath.Assign(ImGuiFileDialog::Instance()->GetFilePathName().c_str());
            mFileName.Assign(ImGuiFileDialog::Instance()->GetCurrentFileName().c_str());
        }
        ImGuiFileDialog::Instance()->Close();
    }
}

bool FTResource::IsReferenced()
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