#include "ResourceSystem/FTResource.h"
#include "FileSystem/ChunkLoader.h"


#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

#include "EditorLayer.h"
 // _DEBUG

void FTResource::SaveProperties(nlohmann::ordered_json& out, UINT key)
{
    FileIOHelper::AddScalarValue(out["Key"], key);
    FileIOHelper::AddScalarValue(out["FileName"], mFileName);
	FileIOHelper::AddScalarValue(out["RelativePath"], mRelativePath);
}

void FTResource::LoadProperties(nlohmann::ordered_json& itemTree)
{
    mFileName.assign(FileIOHelper::LoadScalarValue<std::string>(itemTree, "FileName"));
    mRelativePath.assign(FileIOHelper::LoadScalarValue<std::string>(itemTree, "RelativePath"));
}

void FTResource::UpdateRelativePath(std::string fileExtension)
{
    std::string currentPath = "No path has been assigned";
    mRelativePath.assign(currentPath);
    if (!mRelativePath.empty())
        currentPath = "Current path : \n" + mRelativePath;

    if (ImGui::Button("Select File")) {
        IGFD::FileDialogConfig config;
        config.path = ".";
        config.countSelectionMax = 1;
        ImGuiFileDialog::Instance()->OpenDialog("SelectFile", "Select File", fileExtension.c_str(), config);
        ImGui::OpenPopup("Select File");
    }

    if (ImGuiFileDialog::Instance()->Display("SelectFile"))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
            mRelativePath = ImGuiFileDialog::Instance()->GetFilePathName();
        ImGuiFileDialog::Instance()->Close();
    }
}
