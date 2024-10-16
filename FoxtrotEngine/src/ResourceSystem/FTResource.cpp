#include "ResourceSystem/FTResource.h"

#include <fstream>

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"

#ifdef FOXTROT_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

#include "EditorLayer.h"
#endif //FOXTROT_EDITOR

void FTResource::SaveProperties(std::ofstream& ofs)
{
    FileIOHelper::SaveString(ofs, ChunkKeys::FILE_NAME, mFileName);
    FileIOHelper::SaveString(ofs, ChunkKeys::RELATIVE_PATH, mRelativePath);
}

// When loading properties, invert the order of the member variables
// (Due to the loading order)
void FTResource::LoadProperties(std::ifstream& ifs)
{
    FileIOHelper::LoadBasicString(ifs, mRelativePath);
    FileIOHelper::LoadBasicString(ifs, mFileName);
}

#ifdef FOXTROT_EDITOR
void FTResource::SaveProperties(nlohmann::ordered_json& out, unsigned int key)
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
#endif // FOXTROT_EDITOR