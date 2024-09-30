
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

void EditorLayer::Update(float deltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();
	mSaveKeyPressed    = ImGui::IsKeyChordPressed(ImGuiKey::ImGuiKey_LeftCtrl | ImGuiKey::ImGuiKey_S);
	mSaveAsKeyPressed  = ImGui::IsKeyChordPressed(ImGuiKey::ImGuiKey_LeftCtrl | ImGuiKey::ImGuiKey_LeftShift | ImGuiKey::ImGuiKey_S);
	mOpenKeyPressed	   = ImGui::IsKeyChordPressed(ImGuiKey::ImGuiKey_LeftCtrl | ImGuiKey::ImGuiKey_O);
	mConfirmKeyPressed = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter);
	mUndoKeyPressed	   = ImGui::IsKeyChordPressed(ImGuiKey::ImGuiKey_LeftCtrl | ImGuiKey::ImGuiKey_Z);
	mRedoKeyPressed    = ImGui::IsKeyChordPressed(ImGuiKey::ImGuiKey_LeftCtrl | ImGuiKey::ImGuiKey_LeftShift | ImGuiKey::ImGuiKey_Z);
	mDeleteKeyPressed  = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete);

	CommandHistory::GetInstance()->Update();
	
	DisplayViewport();
	DisplayFileMenu();
	DisplayHierarchyMenu();
	DisplayResourceMenu();
	DisplayInspectorMenu();
	Camera::GetInstance()->DisplayCameraMenu();
	ApplyCommandHistory();
	DisplayErrorMessage();
	ImGui::EndFrame();
}

void EditorLayer::Render(FoxtrotRenderer* renderer)
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	//ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

void EditorLayer::DisplayEditorElements(FoxtrotRenderer* renderer)
{
	for (int i = 0; i < mEditorElements.size(); ++i)
		mEditorElements[i]->Render(renderer);
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
	renderer->RenderToTexture();
	ID3D11ShaderResourceView* viewportTexture = renderer->GetRenderTexture()->GetShaderResourceView().Get();
	ImVec2 viewportSize = ImVec2(renderer->GetRenderWidth(), renderer->GetRenderHeight());
	ImGui::Image((void*)viewportTexture, viewportSize);
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
			ImGuiFileDialog::Instance()->OpenDialog("SaveChunkFile", "Save As", CHUNK_FILE_FORMAT, config);
		}
		else if (selection == fileMenu[4] || mOpenKeyPressed)
		{
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.countSelectionMax = 1;
			ImGuiFileDialog::Instance()->OpenDialog("OpenChunkFile", "Open Chunk", CHUNK_FILE_FORMAT, config);
		}

		if (ImGui::Button("New Game Object"))
		{
			Scene* currScene = SceneManager::GetInstance()->GetCurrScene();
			AddEditorElement(currScene);
		}
		if (ImGui::Button("Play"))
		{
			if (mCurrFileSaved)
			{
				if (!mCurrFilePath.empty())
				{
					EditorChunkLoader::GetInstance()->SaveChunk(mCurrFilePath);
					this->DeleteAll();
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
					SceneManager::GetInstance()->GetCurrScene()->DeleteAll();
					this->DeleteAll();
					EditorChunkLoader::GetInstance()->LoadChunk(mCurrFilePath);
				}
			}
		}
		if (ImGui::Button("2D"))
		{
			//FoxtrotRenderer* renderer = FTCoreEditor::GetInstance()->GetGameRenderer();
			//if(renderer->GetViewType() == Viewtype::Perspective)
			//	renderer->SetViewType(Viewtype::Orthographic);
			//else if (renderer->GetViewType() == Viewtype::Orthographic)
			//	renderer->SetViewType(Viewtype::Perspective);
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
				mCurrFileSaved = true;
			}
			ImGuiFileDialog::Instance()->Close();
		}

		if (ImGuiFileDialog::Instance()->Display("OpenChunkFile"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				this->DeleteAll();
				mCurrFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
				EditorChunkLoader::GetInstance()->LoadChunk(mCurrFilePath);
				LogInt(mEditorElements.size());
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
		std::string* actorNames = new std::string[mEditorElements.size()];
		for (UINT i = 0; i < mEditorElements.size(); ++i)
		{
			actorNames[i] = ToString(mEditorElements[i]->GetName());
		}
		bool* selection = new bool[mEditorElements.size()];
		for (UINT i = 0; i < mEditorElements.size(); ++i)
		{
			if (ImGui::Selectable(actorNames[i].c_str(), mActorNameIdx == i))
			{
				CommandHistory::GetInstance()->AddCommand(new IntEditCommand(mActorNameIdx, i));
				mActorNameIdx = i;
			}
		}
		delete[] actorNames;
		ImGui::EndListBox();
	}
	//ResizeUIWindow(menuID);
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
	for (int i = 0; i < mEditorElements.size(); ++i)
	{
		//mEditorElements[i]->EditorUpdateComponents(deltaTime);
		if (mEditorElements[i]->GetIsFocused())
		{
			mEditorElements[i]->UpdateUI();
			if (mDeleteKeyPressed)
			{
				// Delete game object, and erase the pointed from std::vector
				delete mEditorElements[i];
				mEditorElements.erase(mEditorElements.begin() + i);
			}
		}
	}
	//ResizeUIWindow(menuID);
	ImGui::End();
}

void EditorLayer::ApplyCommandHistory()
{
	if (mActorNameIdx < mEditorElements.size())
	{
		UnfocusEditorElements();
		mEditorElements[mActorNameIdx]->SetIsFocused(true);
	}
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
		if (!mCurrFilePath.empty())
			EditorChunkLoader::GetInstance()->SaveChunk(mCurrFilePath);
		else
			LogString("Saved file path doesn't exist but trying to access it");
	}
	else
	{
		IGFD::FileDialogConfig config;
		config.path = ".";
		config.countSelectionMax = 1;
		config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
		ImGuiFileDialog::Instance()->OpenDialog("SaveChunkFile", "Save", CHUNK_FILE_FORMAT, config);
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

// Adds empty EditorElements
void EditorLayer::AddEditorElement(Scene* scene)
{
	UnfocusEditorElements();
	EditorElement* editorElement = new EditorElement(scene);
	editorElement->SetName(L"Game Object" + std::to_wstring(++mElementNumber));
	editorElement->SetIsFocused(true);
	mEditorElements.emplace_back(editorElement);
}

void EditorLayer::AddEditorElement(Actor* actor)
{
	UnfocusEditorElements();
	EditorElement* editorElement = new EditorElement(actor);
	mEditorElements.emplace_back(editorElement);
	++mElementNumber;
}

void EditorLayer::DeleteAll()
{
	for (size_t i = 0; i < mEditorElements.size(); ++i)
		delete mEditorElements[i];
	mEditorElements.clear();
}

void EditorLayer::UnfocusEditorElements()
{
	for (int i = 0; i < mEditorElements.size(); ++i)
	{
		if (mEditorElements[i]->GetIsFocused())
			mEditorElements[i]->SetIsFocused(false);
	}
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
	:
	//mViewportSize()
	mEditorElements{}
	, mCurrFileSaved(false)
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
{
	// Initial command stored in front of every following commands
	CommandHistory::GetInstance()->AddCommand(new IntEditCommand(mActorNameIdx, 0));
}

EditorLayer::~EditorLayer()
{}
