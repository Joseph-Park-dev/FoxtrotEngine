#include "FoxtrotEngine/ResourceSystem/FTResource.h"
#include "FoxtrotEngine/FileSystem/ChunkLoader.h"

#ifdef _DEBUG
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

#include "FoxtrotEditor/EditorLayer.h"
#endif // _DEBUG

void FTResource::SaveProperties(nlohmann::ordered_json& out)
{
	FileIOHelper::AddScalarValue(out["RelativePath"], mRelativePath);
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
