#include "EditorLayer.h"

#include <unordered_map>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
#include <imgui_internal.h>
#include <d3d11.h>

#include "EditorElement.h"
#include "FTCoreEditor.h"
#include "EditorChunkLoader.h"
#include "CommandHistory.h"
#include "Command.h"
#include "RenderTextureClass.h"
#include "EditorSceneManager.h"
#include "EditorScene.h"
#include "DebugGeometries.h"

#include "Core/FTCore.h"
#include "Managers/SceneManager.h"
#include "Managers/KeyInputManager.h"
#include "Managers/ResourceManager.h"
#include "Scenes/Scene.h"
#include "Actors/Actor.h"
#include "Actors/ActorGroup.h"
#include "Actors/UIs/PanelUI.h"
#include "Actors/Transform.h"
#include "Actors/Backgrounds/BackgroundActor.h"
#include "Components/Rigidbody2DComponent.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "FileSystem/ChunkFileKeys.h"

void EditorLayer::Update(float deltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();
	mSaveKeyPressed			= ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_S);
	mSaveAsKeyPressed		= ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_LeftShift | ImGuiKey::ImGuiKey_S);
	mOpenKeyPressed			= ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_O);
	mConfirmKeyPressed		= ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter);
	mUndoKeyPressed			= ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_Z);
	mRedoKeyPressed			= ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey::ImGuiKey_Z);
	mDeleteKeyPressed		= ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete);
	mDuplicateKeyPressed	= ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_D);

	CommandHistory::GetInstance()->Update();
	
	DisplayFileMenu();
	DisplayHierarchyMenu();
	DisplayResourceMenu();
	DisplayInspectorMenu();
	Camera::GetInstance()->DisplayCameraMenu();
	//ApplyCommandHistory();
	DisplayInfoMessage();
	DisplayErrorMessage();

	DisplayViewport();
	ImGui::EndFrame();
}

void EditorLayer::DisplayViewport()
{
	ImGui::Begin("Scene");
	mSceneViewportPos = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	FoxtrotRenderer* renderer = FTCoreEditor::GetInstance()->GetGameRenderer();
	if (MOUSE_HOLD(MOUSE::MOUSE_LEFT) && SceneViewportSizeChanged())
	{
		mIsResizingViewport = true;
	}
	if (mIsResizingViewport && MOUSE_AWAY(MOUSE::MOUSE_LEFT))
	{
		//renderer->ResizeSceneViewport(mSceneViewportSize.x, mSceneViewportSize.y);
		renderer->SetRenderWidth(mSceneViewportSize.x);
		renderer->SetRenderHeight(mSceneViewportSize.y);
		//renderer->UpdateDepthBuffer(mSceneViewportSize.x, mSceneViewportSize.y);
		mIsResizingViewport = false;
	}
	ID3D11ShaderResourceView* viewportTexture = renderer->GetRenderTexture()->GetShaderResourceView().Get();
	ImVec2 viewportSize = ImVec2(renderer->GetRenderWidth(), renderer->GetRenderHeight());
	ImGui::Image((ImTextureID)viewportTexture, viewportSize);

	ImGui::End();
}

void EditorLayer::DisplayFileMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		const size_t maxMenuEle = 5;
		const std::string fileMenu[maxMenuEle] = { "New Project", "Open Project", "Save", "Save As", "Open" };

		std::string selection = {};
		if (ImGui::Button("File"))
		{
			ImGui::OpenPopup("FilePopUp");
		}

		if (ImGui::BeginPopup("FilePopUp"))
		{
			for (size_t i = 0; i < maxMenuEle; ++i)
				if (ImGui::Selectable(fileMenu[i].c_str()))
					selection = fileMenu[i];
			ImGui::EndPopup();
		}

		if (selection == fileMenu[0])
		{
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.countSelectionMax = 1;
			config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
			ImGuiFileDialog::Instance()->OpenDialog("NewProject", "New Project", "", config);
		}
		else if (selection == fileMenu[1])
		{
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.countSelectionMax = 1;
			config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
			ImGuiFileDialog::Instance()->OpenDialog("OpenProject", "Open Project", "", config);
		}
		// Save menu
		else if (selection == fileMenu[2] || mSaveKeyPressed)
		{
			SaveChunkFromUI();
		}
		else if (selection == fileMenu[3] || mSaveAsKeyPressed)
		{
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.countSelectionMax = 1;
			config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
			ImGuiFileDialog::Instance()->OpenDialog("SaveChunkFile", "Save As", ChunkKeys::CHUNK_FILE_FORMAT, config);
		}
		else if (selection == fileMenu[4] || mOpenKeyPressed)
		{
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.countSelectionMax = 1;
			ImGuiFileDialog::Instance()->OpenDialog("OpenChunkFile", "Open Chunk", ChunkKeys::CHUNK_FILE_FORMAT, config);
		}

		if (ImGui::Button("New Game Object"))
		{
			EditorSceneManager::GetInstance()->GetEditorScene()->AddEditorElement();
		}
		if (ImGui::Button("Play"))
		{
			if (mCurrFileSaved)
			{
				if (!mCurrFilePath.empty())
				{
					EditorChunkLoader::GetInstance()->SaveChunk(mCurrFilePath);
					DebugGeometries::GetInstance()->DeleteAll();
					EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();
					ResourceManager::GetInstance()->DeleteAll();
					ResourceManager::GetInstance()->Initialize(FTCoreEditor::GetInstance()->GetGameRenderer());
					EditorChunkLoader::GetInstance()->LoadChunk(mCurrFilePath);
					FTCoreEditor::GetInstance()->SetIsUpdatingGame(true);
				}
				else
					LogString("Saved file path doesn't exist but trying to access it");
			}
			else
				LogString("Chunk file must be saved before playing");
		}
		if (ImGui::Button("Stop"))
		{
			if (mCurrFileSaved)
			{
				if (!mCurrFilePath.empty())
				{
					FTCoreEditor::GetInstance()->SetIsUpdatingGame(false);
					DebugGeometries::GetInstance()->DeleteAll();
					EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();
					ResourceManager::GetInstance()->DeleteAll();
					ResourceManager::GetInstance()->Initialize(FTCoreEditor::GetInstance()->GetGameRenderer());
					EditorChunkLoader::GetInstance()->LoadChunk(mCurrFilePath);
				}
			}
		}
		if (ImGui::Button("2D"))
		{
			FoxtrotRenderer* renderer = FTCoreEditor::GetInstance()->GetGameRenderer();
			if(Camera::GetInstance()->GetViewType() == Viewtype::Perspective)
				Camera::GetInstance()->SetViewType(Viewtype::Orthographic);
			else if (Camera::GetInstance()->GetViewType() == Viewtype::Orthographic)
				Camera::GetInstance()->SetViewType(Viewtype::Perspective);
		}
		if (ImGui::Button("Wireframe"))
		{
			FoxtrotRenderer* renderer = FTCoreEditor::GetInstance()->GetGameRenderer();
			if (renderer->GetFillMode() == FillMode::Solid)
				renderer->SetFillMode(FillMode::WireFrame);
			else if (renderer->GetFillMode() == FillMode::WireFrame)
				renderer->SetFillMode(FillMode::Solid);
		}

		if (ImGuiFileDialog::Instance()->Display("NewProject"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				mCurrProjectPath = ImGuiFileDialog::Instance()->GetFilePathName();
				if (!ProjectPathExists(mCurrProjectPath))
				{
					// Create base directory for a project
					std::filesystem::create_directory(mCurrProjectPath);
					// Create sub directories - "./Assets"
					std::filesystem::create_directory(mCurrProjectPath + "/Assets");
					std::filesystem::create_directory(mCurrProjectPath + "/FoxtrotEngine_ProjectData");
				}
				else {
					mErrorType = ErrorType::ProjectPathExists;
				}
			}
			ImGuiFileDialog::Instance()->Close();
		}

		if (ImGuiFileDialog::Instance()->Display("OpenProject"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
				if (ProjectPathExists(path)) {
					mCurrProjectPath.assign(path);
				}
				else {
					mErrorType = ErrorType::ProjectNotValid;
				}
			}
			ImGuiFileDialog::Instance()->Close();
		}

		if (ImGuiFileDialog::Instance()->Display("SaveChunkFile"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				mCurrFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
				EditorChunkLoader::GetInstance()->SaveChunk(mCurrFilePath);
				mInfoType = InfoType::ChunkIsSaved;
				mCurrFileSaved = true;
			}
			ImGuiFileDialog::Instance()->Close();
		}

		if (ImGuiFileDialog::Instance()->Display("OpenChunkFile"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();
				mCurrFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
				EditorChunkLoader::GetInstance()->LoadChunk(mCurrFilePath);
				mCurrFileSaved = true;
			}
			ImGuiFileDialog::Instance()->Close();
		}
		if (ImGui::IsKeyPressed(ImGuiKey_Escape))
		{
			ImGuiFileDialog::Instance()->Close();
		}
		ImGui::EndMainMenuBar();
	}
}

void EditorLayer::DisplayHierarchyMenu()
{
	std::string menuID = "Hierarchy Menu";
	ImGui::Begin(menuID.c_str());
	if (ImGui::BeginListBox("Hierarchy", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
		size_t eleSize = scene->GetActorCount();

		std::vector<std::string> actorNames;
		actorNames.reserve(eleSize);

		for (size_t i = 0; i < (size_t)ActorGroup::END; ++i) 
		{
			size_t size = scene->GetActorGroup(i).size();
			if (0 < size)
			{
				for (int j = 0; j < size; ++j)
				{
					Actor* actor = scene->GetActorGroup(i)[j];
					actorNames.push_back(actor->GetName());
				}
			}
		}
		for (UINT i = 0; i < eleSize; ++i)
		{
			if (ImGui::Selectable(actorNames[i].c_str(), mActorNameIdx == i))
			{
				mActorNameIdx = i;
				CommandHistory::GetInstance()->AddCommand(new IntEditCommand(mActorNameIdx, i));
				EditorSceneManager::GetInstance()->GetEditorScene()->UnfocusEditorElements();
				size_t group = i / ((size_t)ActorGroup::END - 1);
				size_t idx = i % ((size_t)ActorGroup::END - 1);
				Actor* actor = scene->GetActorGroup(group)[idx];
				mFocusedEditorElement = dynamic_cast<EditorElement*>(actor);
				mFocusedEditorElement->SetIsFocused(true);
			}
		}
		ImGui::EndListBox();
	}

	if (mDuplicateKeyPressed)
		EditorSceneManager::GetInstance()->GetEditorScene()
			->AddEditorElement(mFocusedEditorElement);
	ImGui::End();
}

void EditorLayer::DisplayResourceMenu()
{
	std::string menuID = "Resource Manager";
	ImGui::Begin(menuID.c_str());
	ResourceManager::GetInstance()->UpdateUI();
	ImGui::End();
}

void EditorLayer::DisplayInspectorMenu()
{
	std::string menuID = "Inspector";
	ImGui::Begin(menuID.c_str());
	EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
	std::vector<Actor*>* actors = scene->GetActors();

	size_t group = mActorNameIdx / ((size_t)ActorGroup::END - 1);
	size_t idx = mActorNameIdx % ((size_t)ActorGroup::END - 1);

	if (0 < scene->GetActorCount())
	{
		Actor* actor = scene->GetActorGroup(group)[idx];
		EditorElement* ele = dynamic_cast<EditorElement*>(actor);
		if (ele->GetIsFocused())
		{
			ele->UpdateUI(false);
			if (mDeleteKeyPressed)
			{
				// Delete game object, and erase the pointed from std::vector
				delete ele;
				actors[group].erase(actors[group].begin() + idx);
				if(0 < mActorNameIdx)
					mActorNameIdx = scene->GetActorCount()-1;
			}
		}
	}
	//ResizeUIWindow(menuID);
	ImGui::End();
}

bool EditorLayer::SceneViewportSizeChanged()
{
	if (mSceneViewportSize != ImGui::GetContentRegionAvail())
	{
		mSceneViewportSize = ImGui::GetContentRegionAvail();
		return true;
	}
	return false;
}

bool EditorLayer::ProjectPathExists(std::string& projDir)
{
	std::string assetDir = projDir + "\\Assets";
	std::string projDataDir = projDir + "\\FoxtrotEngine_ProjectData";
	return std::filesystem::exists(assetDir) && std::filesystem::exists(projDataDir);
}

void EditorLayer::SaveChunkFromUI()
{
	if (mCurrFileSaved)
	{
		if (!mCurrFilePath.empty()) {
			EditorChunkLoader::GetInstance()->SaveChunk(mCurrFilePath);
			mInfoType = InfoType::ChunkIsSaved;
		}

		else
			LogString("Saved file path doesn't exist but trying to access it");
	}
	else
	{
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
		ImGuiFileDialog::Instance()->OpenDialog("SaveChunkFile", "Save", ChunkKeys::CHUNK_FILE_FORMAT, config);
	}
}

void EditorLayer::DisplayInfoMessage()
{
	switch (mInfoType)
	{
	case InfoType::ChunkIsSaved:
		PopUpInfo_ChunkIsSaved();
		break;
	default:
		break;
	}
}

void EditorLayer::DisplayErrorMessage()
{
	switch (mErrorType)
	{
	case ErrorType::None:
		break;

	case ErrorType::ProjectPathExists:
		PopUpError_ProjectPathExists();
		break;

	case ErrorType::ProjectNotValid:
		PopUpError_ProjectNotValid();
		break;

	case ErrorType::ChunkNotSaved:
		PopUpError_ChunkNotSaved();
		break;

	default:
		break;
	}
}

void EditorLayer::PopUpInfo_ChunkIsSaved()
{
	ImGui::OpenPopup("Info");
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Info", NULL))
	{
		ImGui::Text(".chunk file is saved.");
		ImGui::Separator();
		ImGui::SetItemDefaultFocus();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			mInfoType = InfoType::None;
		}
		ImGui::EndPopup();
	}
}


void EditorLayer::PopUpError_ProjectPathExists()
{
	ImGui::OpenPopup("Project path exists");
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Project path exists", NULL))
	{
		ImGui::Text("Foxtrot Engine project\nalready exists in this directory.");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); mErrorType = ErrorType::None; }
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
}

void EditorLayer::PopUpError_ProjectNotValid()
{
	ImGui::OpenPopup("ProjectNotValid");
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("ProjectNotValid", NULL))
	{
		ImGui::Text("The directory is not a valid Foxtrot Engine Project");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); mErrorType = ErrorType::None;}
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
}

void EditorLayer::PopUpError_ChunkNotSaved()
{
	ImGui::OpenPopup("Chunk is not saved");
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Chunk is not saved", NULL))
	{
		ImGui::Text(".Chunk is not saved.\nSave the file before closing the editor.");
		ImGui::Separator();

		if (ImGui::Button("Discard", ImVec2(120, 0))) { 
			FTCoreEditor::GetInstance()->SetIsRunning(false); 
			ImGui::EndPopup();
			return; 
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) 
		{ 
			ImGui::CloseCurrentPopup(); 
			mErrorType = ErrorType::None;
		}
		ImGui::EndPopup();
	}
}

void EditorLayer::Render(FoxtrotRenderer* renderer)
{
	ImGui::Render();
	//ImDrawData* drawData = ImGui::GetDrawData();
	//
	//ImVec2 screenCenter = mSceneViewportPos + (mSceneViewportSize / 2);
	//EditorSceneManager::GetInstance()->
	//	RenderDebugGeometries(
	//		mDebugDrawList, 
	//		FTVector2(screenCenter.x, screenCenter.y)
	//	);

	//drawData->AddDrawList(mDebugDrawList);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

//void EditorLayer::ResizeUIWindow(std::string menuID)
//{
//	if (ImGui::IsWindowDocked())
//	{
//		if (mUIMap[menuID].first != ImGui::GetWindowPos() ||
//			mUIMap[menuID].second != ImGui::GetWindowSize())
//		{
//			std::pair<ImVec2, ImVec2> windowInfo = std::make_pair(ImGui::GetWindowPos(), ImGui::GetWindowSize());
//			mUIMap[menuID] = windowInfo;
//
//			LogVector2(FTVector2(windowInfo.first.x, windowInfo.first.y));
//		}
//	}
//}

void EditorLayer::ShutDown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// After directX implementation
//void EditorLayer::DisplayViewport()
//{
//	ImGui::Begin("Viewport");
//	ImVec2 contentRegion = ImGui::GetContentRegionAvail();
//	if (mViewportSize != contentRegion)
//		mViewportSize = contentRegion
//	ImGui::End();
//}

EditorLayer::EditorLayer()
	: mCurrFileSaved(false)
	, mCurrFilePath{}
	, mHierarchyIdx(0)
	, mActorNameIdx(0)
	, mSaveKeyPressed(false)
	, mSaveAsKeyPressed(false)
	, mOpenKeyPressed(false)
	, mUndoKeyPressed(false)
	, mRedoKeyPressed(false)
	, mConfirmKeyPressed(false)
	, mDeleteKeyPressed(false)
	, mIsResizingViewport(false)
	, mElementNumber(0)
	, mSceneViewportSize(ImVec2(1920.f, 1080.f))
	, mInfoType(InfoType::None)
	, mErrorType(ErrorType::None)
{
	// Initial command stored in front of every following commands
	CommandHistory::GetInstance()->AddCommand(new IntEditCommand(mActorNameIdx, 0));
}

EditorLayer::~EditorLayer()
{}
