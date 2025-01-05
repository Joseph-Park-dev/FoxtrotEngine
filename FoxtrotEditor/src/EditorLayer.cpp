// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EditorLayer.h"

#include <unordered_map>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include "imgui/FileDialog/imfilebrowser.h"
#include <d3d11.h>

#include "EditorElement.h"
#include "FTCoreEditor.h"
#include "EditorChunkLoader.h"
#include "CommandHistory.h"
#include "Command.h"
#include "RenderTextureClass.h"
#include "EditorSceneManager.h"
#include "EditorScene.h"
#include "DebugShapes.h"
#include "DirectoryHelper.h"

#include "Core/FTCore.h"
#include "Managers/SceneManager.h"
#include "Managers/KeyInputManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/CollisionManager.h"
#include "Scenes/Scene.h"
#include "Actors/Actor.h"
#include "Actors/ActorGroup.h"
#include "Actors/Transform.h"
#include "Components/Rigidbody2DComponent.h"
#include "Components/UIs/PanelUIComponent.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "FileSystem/ChunkFileKeys.h"
#include "FileSystem/FileIOHelper.h"

#include "Core/EventFunctions.h"

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
	DisplayCollisionMenu();
	DisplayInspectorMenu();
	Camera::GetInstance()->DisplayCameraMenu();
	DisplayInfoMessage();
	DisplayErrorMessage();

	DisplayViewport();
	ImGui::EndFrame();
}

void EditorLayer::DisplayViewport()
{
	ImGui::Begin("Scene");
	if (ImGui::IsWindowFocused())
		mCursorOnViewport = true;
	else
		mCursorOnViewport = false;

	FoxtrotRenderer* renderer = FTCoreEditor::GetInstance()->GetGameRenderer();
	mSceneViewportPos = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
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
			ImGui::OpenPopup("FilePopUp");

		if (ImGui::BeginPopup("FilePopUp"))
		{
			for (size_t i = 0; i < maxMenuEle; ++i)
				if (ImGui::Selectable(fileMenu[i].c_str()))
					selection = fileMenu[i];
			ImGui::EndPopup();
		}

		if (selection == fileMenu[0])
		{
			mFileDialog = ImGui::FileBrowser(mDirSelectFlag);
			mFileDialog.SetTitle("New Project");
			mFileDialog.Open();
			mFileMenuEvent = FileMenuEvents::NewProject;
		}
		else if (selection == fileMenu[1])
		{
			mFileDialog = ImGui::FileBrowser(mDirSelectFlag);
			mFileDialog.SetTitle("Open Project");
			mFileDialog.Open();
			mFileMenuEvent = FileMenuEvents::OpenProject;
		}
		else if (selection == fileMenu[2] || mSaveKeyPressed)
		{
			if (CHUNK_IS_SAVED)
			{
				if (!PATH_PROJECT.empty()) {
					EditorChunkLoader::GetInstance()->SaveChunk(PATH_CHUNK);
					printf("Chunk saved to %s", PATH_CHUNK.c_str());
					mInfoType = InfoType::ChunkIsSaved;
				}
				else
					LogString("Saved file path doesn't exist but trying to access it");
			}
			else
			{
				mFileDialog = ImGui::FileBrowser(mFileSelectFlag);
				mFileDialog.SetTitle("Save");
				mFileDialog.SetTypeFilters({ ChunkKeys::CHUNK_FILE_FORMAT });
				mFileDialog.SetDirectory(PATH_PROJECT);
				mFileDialog.Open();
				mFileMenuEvent = FileMenuEvents::Save;
			}
		}
		else if (selection == fileMenu[3] || mSaveAsKeyPressed)
		{
			mFileDialog = ImGui::FileBrowser(mFileSelectFlag);
			mFileDialog.SetTitle("Save As");
			mFileDialog.SetTypeFilters({ ChunkKeys::CHUNK_FILE_FORMAT });
			mFileDialog.SetDirectory(PATH_PROJECT);
			mFileDialog.Open();
			mFileMenuEvent = FileMenuEvents::SaveAs;
		}
		else if (selection == fileMenu[4] || mOpenKeyPressed)
		{
			mFileDialog = ImGui::FileBrowser(mFileSelectFlag);
			mFileDialog.SetTitle("Open Chunk");
			mFileDialog.SetTypeFilters({ ChunkKeys::CHUNK_FILE_FORMAT });
			mFileDialog.SetDirectory(PATH_PROJECT);
			mFileDialog.Open();
			mFileMenuEvent = FileMenuEvents::Open;
		}

		if (ImGui::Button("New Game Object"))
		{
			EditorSceneManager::GetInstance()->GetEditorScene()->AddEditorElement();
		}
		if (ImGui::Button("Play"))
		{
			if (CHUNK_IS_SAVED)
			{
				if (!PATH_CHUNK.empty())
				{
					EditorChunkLoader::GetInstance()->SaveChunk(PATH_CHUNK);
					DebugShapes::GetInstance()->DeleteAll();
					ResourceManager::GetInstance()->DeleteAll();
					CollisionManager::GetInstance()->Reset();
					EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();
					ResourceManager::GetInstance()->Initialize(FTCoreEditor::GetInstance()->GetGameRenderer());
					EditorChunkLoader::GetInstance()->LoadChunk(PATH_CHUNK);
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
			if (CHUNK_IS_SAVED)
			{
				if (!PATH_CHUNK.empty())
				{
					FTCoreEditor::GetInstance()->SetIsUpdatingGame(false);
					DebugShapes::GetInstance()->DeleteAll();
					ResourceManager::GetInstance()->DeleteAll();
					CollisionManager::GetInstance()->Reset();
					EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();
					ResourceManager::GetInstance()->Initialize(FTCoreEditor::GetInstance()->GetGameRenderer());
					ResourceManager::GetInstance()->LoadAllResourcesInAsset();
					EditorChunkLoader::GetInstance()->LoadChunk(PATH_CHUNK);
				}
			}
		}
		if (ImGui::Button("2D"))
		{
			FoxtrotRenderer* renderer = FTCoreEditor::GetInstance()->GetGameRenderer();
			if (Camera::GetInstance()->GetViewType() == Viewtype::Perspective)
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
		ImGui::EndMainMenuBar();
	}
	mFileDialog.Display();


	if (mFileDialog.HasSelected())
	{
		std::filesystem::path path = mFileDialog.GetSelected();
		switch (mFileMenuEvent)
		{
		case FileMenuEvents::None:
			break;

		case FileMenuEvents::NewProject:
			CreateNewProject(path);
			break;

		case FileMenuEvents::OpenProject:
			OpenProject(path);
			break;

		case FileMenuEvents::Save:
			Save(path);
			break;

		case FileMenuEvents::SaveAs:
			SaveAs(path);
			break;

		case FileMenuEvents::Open:
			Open(path);
			break;

		default:
			break;
		}
	}
	mFileDialog.ClearSelected();
}

void EditorLayer::DisplayHierarchyMenu()
{
	std::string menuID = "Hierarchy Menu";
	ImGui::Begin(menuID.c_str());
	if (ImGui::BeginListBox("Hierarchy", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	{
		EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
		size_t eleSize = scene->GetActorCount();

		std::vector<Actor*> actorsRow;
		actorsRow.reserve(eleSize);

		for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i) 
		{
			size_t size = scene->GetActorGroup(i).size();
			if (0 < size)
			{
				for (int j = 0; j < size; ++j)
					actorsRow.push_back(scene->GetActorGroup(i)[j]);
			}
		}

		for (size_t i = 0; i < eleSize; ++i)
		{
			if (ImGui::Selectable(actorsRow.at(i)->GetName().c_str(), mActorNameIdx == i))
			{
				mActorNameIdx = i;
				CommandHistory::GetInstance()->AddCommand(DBG_NEW IntEditCommand(mActorNameIdx, i));
				EditorSceneManager::GetInstance()->GetEditorScene()->UnfocusEditorElements();
				Actor* actor = actorsRow[mActorNameIdx];
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

void EditorLayer::DisplayCollisionMenu()
{
	std::string menuID = "Collision Manager";
	ImGui::Begin(menuID.c_str());
	CollisionManager::GetInstance()->UpdateUI();
	ImGui::End();
}

void EditorLayer::DisplayInspectorMenu()
{
	std::string menuID = "Inspector";
	ImGui::Begin(menuID.c_str());
	EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
	size_t eleSize = scene->GetActorCount();
	if (0 < eleSize)
	{
		std::vector<Actor*> actorsRow;
		actorsRow.reserve(eleSize);

		for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
		{
			size_t size = scene->GetActorGroup(i).size();
			if (0 < size)
			{
				for (int j = 0; j < size; ++j)
					actorsRow.push_back(scene->GetActorGroup(i)[j]);
			}
		}

		Actor* actor = actorsRow[mActorNameIdx];
		EditorElement* ele = dynamic_cast<EditorElement*>(actor);
		if (ele->GetIsFocused())
		{
			ele->UpdateUI(false);
			if (mDeleteKeyPressed)
			{
				// Delete game object, and erase the pointed from std::vector
				ActorGroup group = ele->GetActorGroup();
				
				std::vector<Actor*>::iterator iter = 
					std::find(scene->GetActorGroup(group).begin(), scene->GetActorGroup(group).end(), actor);
				scene->GetActorGroup(group).erase(iter);
				if (0 < mActorNameIdx)
					mActorNameIdx = scene->GetActorCount() - 1;
				delete ele;
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

bool EditorLayer::ProjectExists(std::string projDir)
{
	return
		std::filesystem::exists(projDir + "//Assets") &&
		std::filesystem::exists(projDir + "//Builds") &&
		std::filesystem::exists(projDir + "//Chunks") &&
		std::filesystem::exists(projDir + "//FoxtrotEngine");
}

void EditorLayer::DisplayInfoMessage()
{
	switch (mInfoType)
	{
	case InfoType::ChunkIsSaved:
		PopUpInfo(".chunk is saved.");
		break;
	case InfoType::PremadeIsCreated:
	{
		auto onConfirm = [this]()
			-> void
			{
				FTPremade* newPremade = DBG_NEW FTPremade();
				newPremade->Create(mFocusedEditorElement);
				ImGui::CloseCurrentPopup();
				mInfoType = InfoType::None;
			};
		if (mFocusedEditorElement)
		{
			std::string msg = "Create Premade with name : " + mFocusedEditorElement->GetName() + "?";
			PopUpInfo("Create Premade", msg.c_str(), onConfirm);
		}
	}
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
		PopUpError(
			"Project path exists", 
			"Foxtrot Engine project\nalready exists in this directory."
		);
		break;

	case ErrorType::ProjectNotValid:
		PopUpError(
			"Project Not Valid", 
			"The directory is not a valid Foxtrot Engine Project"
		);
		break;

	case ErrorType::ProjectPathNotEmpty:
		PopUpError(
			"Folder Not Empty", 
			"The directory is not empty to create a FT Engine Project."
		);
		break;

	case ErrorType::ChunkNotSaved:
	{
		std::function<void()> onConfirm = [this]()
			-> void
			{
				FTCoreEditor::GetInstance()->SetIsRunning(false);
				mErrorType = ErrorType::None;
				ImGui::CloseCurrentPopup();
			};

		std::function<void()> onCancel = [this]()
			-> void
			{
				mErrorType = ErrorType::None;
				ImGui::CloseCurrentPopup();
			};

		PopUpInquiry(
			"Chunk is not saved",
			".Chunk is not saved.\n Discard the chunk?",
			onConfirm,
			onCancel
		);
	}
		break;

	default:
		break;
	}
}

void EditorLayer::PopUpInfo(const char* msg)
{
	ImGui::OpenPopup("Info");
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Info", NULL))
	{
		ImGui::Text(msg);
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

void EditorLayer::PopUpError(const char* title, const char* msg)
{
	ImGui::OpenPopup(title);
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(title, NULL))
	{
		ImGui::Text(msg);
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0))) 
		{ 
			ImGui::CloseCurrentPopup(); 
			mErrorType = ErrorType::None; 
		}
		ImGui::SetItemDefaultFocus();
		ImGui::EndPopup();
	}
}

void EditorLayer::CreateNewProject(std::filesystem::path& path)
{
	bool projExists = ProjectExists(path.string());
	bool pathIsEmpty = std::filesystem::is_empty(path);

	if (!projExists && pathIsEmpty)
	{
		PATH_PROJECT.assign(path.string());
		std::filesystem::create_directory(PATH_PROJECT + "\\Assets");
		std::filesystem::create_directory(PATH_PROJECT + "\\Builds");
		std::filesystem::create_directory(PATH_PROJECT + "\\Chunks");
		std::filesystem::create_directory(PATH_PROJECT + "\\FoxtrotEngine");
		std::ofstream ofs(PATH_PROJECT + "\\FoxtrotEngine" + "\\GameData" + ChunkKeys::GAMEDATA_FILE_FORMAT);
		FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::GAME_DATA);
		FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::CHUNK_LIST);
		FileIOHelper::EndDataPackSave(ofs, ChunkKeys::CHUNK_LIST);
		FileIOHelper::EndDataPackSave(ofs, ChunkKeys::GAME_DATA);
		FileIOHelper::SaveBufferToFile(ofs);
	}
	else {
		if (projExists)
			mErrorType = ErrorType::ProjectPathExists;
		else if (!pathIsEmpty)
			mErrorType = ErrorType::ProjectPathNotEmpty;
	}
	ResourceManager::GetInstance()->DeleteAll();
}

void EditorLayer::OpenProject(std::filesystem::path& path)
{
	if (ProjectExists(path.string())) {
		EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();
		DebugShapes::GetInstance()->DeleteAll();
		ResourceManager::GetInstance()->DeleteAll();
		PATH_PROJECT.assign(path.string());
		ResourceManager::GetInstance()->SetPathToAsset(std::move(PATH_PROJECT));
		ResourceManager::GetInstance()->Initialize(FTCoreEditor::GetInstance()->GetGameRenderer());
		ResourceManager::GetInstance()->LoadAllResourcesInAsset();
	}
	else {
		mErrorType = ErrorType::ProjectNotValid;
	}
}

void EditorLayer::Save(std::filesystem::path& path)
{
	PATH_CHUNK.assign(path.string());
	EditorChunkLoader::GetInstance()->SaveChunk(PATH_CHUNK);
	mInfoType = InfoType::ChunkIsSaved;
	SET_CHUNK_IS_SAVED(true)
}

void EditorLayer::SaveAs(std::filesystem::path& path)
{
	PATH_CHUNK.assign(path.string());
	EditorChunkLoader::GetInstance()->SaveChunk(PATH_CHUNK);
	mInfoType = InfoType::ChunkIsSaved;
	SET_CHUNK_IS_SAVED(true)
}

void EditorLayer::Open(std::filesystem::path& path)
{
	EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();
	PATH_CHUNK.assign(path.string());
	EditorChunkLoader::GetInstance()->LoadChunk(PATH_CHUNK);
	SET_CHUNK_IS_SAVED(true)
}

void EditorLayer::Render(FoxtrotRenderer* renderer)
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void EditorLayer::ShutDown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool EditorLayer::CursorOnViewport() const
{
	return mCursorOnViewport;
}

EditorLayer::EditorLayer()
	: mHierarchyIdx(0)
	, mActorNameIdx(0)
	, mSaveKeyPressed(false)
	, mSaveAsKeyPressed(false)
	, mOpenKeyPressed(false)
	, mUndoKeyPressed(false)
	, mRedoKeyPressed(false)
	, mConfirmKeyPressed(false)
	, mDeleteKeyPressed(false)
	, mDuplicateKeyPressed(false)
	, mIsResizingViewport(false)
	, mCursorOnViewport(false)
	, mFocusedEditorElement(nullptr)
	, mSceneViewportSize(ImVec2(1920.f, 1080.f))
	, mInfoType(InfoType::None)
	, mFileMenuEvent(FileMenuEvents::None)
	, mErrorType(ErrorType::None)
{
	// Initial command stored in front of every following commands
	CommandHistory::GetInstance()->AddCommand(DBG_NEW IntEditCommand(mActorNameIdx, 0));
}

EditorLayer::~EditorLayer()
{}