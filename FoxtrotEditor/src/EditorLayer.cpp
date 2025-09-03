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
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <d3d11.h>

#include "EditorElement.h"
#include "FTCoreEditor.h"
#include "EditorChunkLoader.h"
#include "CommandHistory.h"
#include "Command.h"
#include "ViewportRenderer.h"
#include "EditorSceneManager.h"
#include "EditorScene.h"
#include "DirectoryHelper.h"
#include "EditorCamera.h"
#include "EditorResourceManager.h"

#include "Core/FTCore.h"
#include "Managers/DebugShapes.h"
#include "Managers/SceneManager.h"
#include "InputSystem/FTInputDevice.h"
#include "Managers/ResourceManager.h"
#include "Managers/CollisionManager.h"
#include "Managers/UIManager.h"
#include "Managers/LightManager.h"
#include "Managers/AnimationManager.h"
#include "Managers/TileMapManager.h"
#include "Managers/SoundManager.h"
#include "Scenes/Scene.h"
#include "Actors/Actor.h"
#include "Actors/ActorGroup.h"
#include "Actors/Transform.h"
#include "Components/Rigidbody2D.h"
#include "Components/UIs/PanelUI.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/Camera.h"
#include "WindowSystem/FTWindow.h"
#include "Renderer/FTRectArea.h"
#include "FileSystem/FileIOHelper.h"
#include "ResourceSystem/FTPremade.h"
#include "ResourceSystem/FTRectangle.h"

#include "Core/EventFunctions.h"

void EditorLayer::Update(float deltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();
	mSaveKeyPressed		 = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_S);
	mSaveAsKeyPressed	 = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_LeftShift | ImGuiKey::ImGuiKey_S);
	mOpenKeyPressed		 = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_O);
	mConfirmKeyPressed	 = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter);
	mUndoKeyPressed		 = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_Z);
	mRedoKeyPressed		 = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey::ImGuiKey_Z);
	mDeleteKeyPressed	 = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete);
	mDuplicateKeyPressed = ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey::ImGuiKey_D);

	CommandHistory::GetInstance()->Update();

	DisplayMainMenuBar();
	DisplayHierarchyMenu();
	DisplayResourceMenu();
	DisplayCollisionMenu();
	DisplayInspectorMenu();

	ImGui::Begin("Camera Menu");
	EditorCamera::GetInstance()->DisplayMainCameraMenu();
	EditorCamera::GetInstance()->DisplayEditorCameraMenu();
	ImGui::End();

	LightManager::GetInstance()->DisplayLightMenu();
	DisplayInfoMessage();
	DisplayErrorMessage();

	DisplayViewport();
	DisplayFrameRate();
	ImGui::EndFrame();
}

void EditorLayer::DisplayViewport()
{
	ImGui::Begin("Scene");
	if (ImGui::IsWindowFocused())
		mCursorOnViewport = true;
	else
		mCursorOnViewport = false;

	ImVec2 windowPos  = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
	ImVec2 contentReg = ImGui::GetContentRegionAvail();
		
	FTWindow*		 editorWin = FTCoreEditor::GetInstance()->GetEditorWindow();
	FoxtrotRenderer* renderer  = FTCoreEditor::GetInstance()->GetGameRenderer();

	if (editorWin->MOUSE_HOLD(MOUSE::MOUSE_LEFT) && 
		SceneViewportSizeChanged(editorWin->GetRenderArea()->GetSize().GetImVec2()))
	{
		mIsResizingViewport = true;
	}
	if (mIsResizingViewport && editorWin->MOUSE_AWAY(MOUSE::MOUSE_LEFT))
	{
		editorWin->GetRenderArea()->Set(0.f, 0.f, contentReg.x, contentReg.y);
		renderer->GetViewportRenderer()->InitializeTexture(renderer, contentReg);
		mIsResizingViewport = false;
	}

	ID3D11ShaderResourceView* viewportTexture = renderer->GetViewportRenderer()->GetViewportSRV().Get();
	ImGui::Image((ImTextureID)(intptr_t)viewportTexture, contentReg);

	ImGui::End();
}

void EditorLayer::DisplayFrameRate()
{
	ImGui::Begin("Frame Rate");
	// Display current frame rate
	ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}

void EditorLayer::DisplayMainMenuBar()
{
	const size_t	  maxMenuEle		   = 5;
	const std::string fileMenu[maxMenuEle] = { "New Project", "Open Project", "Save", "Save As", "Open" };

	IGFD::FileDialogConfig config;
	config.flags = ImGuiFileDialogFlags_Modal;

	if (ImGui::BeginMainMenuBar())
	{
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
			ImGuiFileDialog::Instance()->OpenDialog(fileMenu[0], "Choose Directory", nullptr, config);
			mFileMenuEvent = FileMenuEvents::NewProject;
		}
		else if (selection == fileMenu[1])
		{
			ImGuiFileDialog::Instance()->OpenDialog(fileMenu[1], "Choose Directory", nullptr, config);
			mFileMenuEvent = FileMenuEvents::OpenProject;
		}
		else if (selection == fileMenu[2] || mSaveKeyPressed)
		{
			if (CHUNK_IS_SAVED)
			{
				if (!PATH_PROJECT.IsEmpty())
				{
					EditorChunkLoader::GetInstance()->SaveChunk(PATH_CHUNK);
					printf("Chunk saved to %s", PATH_CHUNK);
					mInfoType = InfoType::ChunkIsSaved;
				}
				else
					LogString("Saved file path doesn't exist but trying to access it");
			}
			else
			{
				IGFD::FileDialogConfig config;
				config.path = PATH_PROJECT.C_Str();
				ImGuiFileDialog::Instance()->OpenDialog(fileMenu[2], "Choose Directory", FileTypes::CHUNK, config);
				mFileMenuEvent = FileMenuEvents::Save;
			}
		}
		else if (selection == fileMenu[3] || mSaveAsKeyPressed)
		{
			IGFD::FileDialogConfig config;
			config.path = PATH_PROJECT.C_Str();
			ImGuiFileDialog::Instance()->OpenDialog(fileMenu[3], "Choose Directory", FileTypes::CHUNK, config);
			mFileMenuEvent = FileMenuEvents::SaveAs;
		}
		else if (selection == fileMenu[4] || mOpenKeyPressed)
		{
			if (!PATH_PROJECT.IsEmpty())
			{
				IGFD::FileDialogConfig config;
				config.path = PATH_PROJECT.C_Str();
				ImGuiFileDialog::Instance()->OpenDialog(fileMenu[4], "Choose File", FileTypes::CHUNK, config);
				mFileMenuEvent = FileMenuEvents::Open;
			}
		}

		DisplayManagersMenu();

		if (ImGui::Button("New Empty Actor"))
			EditorSceneManager::GetInstance()->GetEditorScene()->AddEditorElement();

		if (ImGui::Button("Play"))
		{
			if (CHUNK_IS_SAVED)
			{
				if (!PATH_CHUNK.IsEmpty())
				{
					// Clear up the scene to load the current chunk file.
					mFocusedEditorElement = nullptr;
					EditorChunkLoader::GetInstance()->SaveChunk(PATH_CHUNK);
					DebugShapes::GetInstance()->DeleteAll();
					UIManager::GetInstance()->Reset();
					CollisionManager::GetInstance()->Reset();
					EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();

					// Copy the chunk file.
					EditorChunkLoader::GetInstance()->CopyChunk(PATH_CHUNK);
					FTDS::String& copiedPath = EditorChunkLoader::GetInstance()->CurrentChunk();

					// Load the copied chunk file.
					EditorChunkLoader::GetInstance()->LoadChunk(copiedPath);

					// Start updating the game.
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
				if (!PATH_CHUNK.IsEmpty())
				{
					// Delete the created copy.
					EditorChunkLoader::GetInstance()->DeleteCopiedChunk();

					// Clear up the scene to load the current chunk file.
					mFocusedEditorElement = nullptr;
					FTCoreEditor::GetInstance()->SetIsUpdatingGame(false);
					DebugShapes::GetInstance()->DeleteAll();
					CollisionManager::GetInstance()->Reset();
					EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();

					// Reload current scene again.
					EditorChunkLoader::GetInstance()->LoadChunk(PATH_CHUNK);
				}
			}
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

	switch (mFileMenuEvent)
	{
		case FileMenuEvents::None:
			ImGuiFileDialog::Instance()->Close();
			break;

		case FileMenuEvents::NewProject:
		{
			if (ImGuiFileDialog::Instance()->Display(fileMenu[0]))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string path = ImGuiFileDialog::Instance()->GetCurrentPath();
					CreateNewProject(path);
				}
				mFileMenuEvent = FileMenuEvents::None;
			}
			break;
		}

		case FileMenuEvents::OpenProject:
		{
			if (ImGuiFileDialog::Instance()->Display(fileMenu[1]))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string path = ImGuiFileDialog::Instance()->GetCurrentPath();
					OpenProject(path);
				}
				mFileMenuEvent = FileMenuEvents::None;
			}
			break;
		}
		case FileMenuEvents::Save:
		{
			if (ImGuiFileDialog::Instance()->Display(fileMenu[2]))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
					Save(path);
				}
				mFileMenuEvent = FileMenuEvents::None;
			}
			break;
		}
		case FileMenuEvents::SaveAs:
		{
			if (ImGuiFileDialog::Instance()->Display(fileMenu[3]))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
					SaveAs(path);
				}
				mFileMenuEvent = FileMenuEvents::None;
			}
			break;
		}
		case FileMenuEvents::Open:
		{
			if (ImGuiFileDialog::Instance()->Display(fileMenu[4]))
			{
				if (ImGuiFileDialog::Instance()->IsOk())
				{
					std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
					Open(path);
				}
				mFileMenuEvent = FileMenuEvents::None;
			}
			break;
		}
		default:
			break;
	}
}

void EditorLayer::DisplayManagersMenu()
{
	const size_t maxMenuEle			= 2;
	const char*	 menu[maxMenuEle]	= { "Animation Manager", "Sound Manager" };
	static bool	 opened[maxMenuEle] = { false, false };

	if (ImGui::Button("Managers"))
		ImGui::OpenPopup("ManagerPopUp");

	if (ImGui::BeginPopup("ManagerPopUp"))
	{
		for (size_t i = 0; i < maxMenuEle; ++i)
			if (ImGui::Selectable(menu[i]))
				opened[i] = true;
		ImGui::EndPopup();
	}

	if (opened[0])
		AnimationManager::GetInstance()->UpdateUI(&opened[0]);
	if (opened[1])
		SoundManager::GetInstance()->UpdateUI(&opened[2]);
}

void EditorLayer::DisplayHierarchyMenu()
{
	std::string menuID = "Hierarchy Menu";
	ImGui::Begin(menuID.c_str());
	if (ImGui::BeginListBox("Hierarchy", ImVec2(-FLT_MIN, ImGui::GetContentRegionMax().y)))
	{
		std::vector<EditorElement*> lowest;
		EditorSceneManager::GetInstance()->GetLowests(lowest);

		if (0 < lowest.size())
		{
			// Display EditorElements as a list of selections.
			size_t idx = 0;
			for (EditorElement* ele : lowest)
				DisplaySelection(ele, idx);
		}
		ImGui::EndListBox();

		for (EditorElement* ele :
			 EditorSceneManager::GetInstance()->GetEditorScene()->GetEditorElements())
			ele->SetIsDisplayed(false);
	}

	if (mDuplicateKeyPressed)
		EditorSceneManager::GetInstance()->GetEditorScene()->AddEditorElement(mFocusedEditorElement);
	ImGui::End();
}

void EditorLayer::DisplaySelection(EditorElement* element, size_t& index)
{
	FTDS::String indentedName = FTDS::String(element->GetHierarchyLevel(), '\t');
	indentedName.Append(element->GetName());

	if (ImGui::Selectable(indentedName.C_Str(), mActorNameIdx == index))
	{
		mActorNameIdx = index;

		IntEditCommand* command = DBG_NEW IntEditCommand(mActorNameIdx);
		command->SetNextVal(mActorNameIdx);
		CommandHistory::GetInstance()->AddCommand(command);

		EditorSceneManager::GetInstance()->GetEditorScene()->UnfocusEditorElements();
		mFocusedEditorElement = element;
		mFocusedEditorElement->SetIsFocused(true);
	}

	ProcessDragEvent(element);
	ProcessDropEvent(element);
	++index;

	// Recurse to display child Actors in the list.
	if (0 < element->GetChildActors().size())
	{
		for (Actor* child : element->GetChildActors())
		{
			EditorElement* childElem = static_cast<EditorElement*>(child);
			DisplaySelection(childElem, index);
		}
	}
}

void EditorLayer::ProcessDragEvent(EditorElement* from)
{
	if (ImGui::BeginDragDropSource())
	{
		mDraggedEditorElement = static_cast<void*>(from);
		ImGui::SetDragDropPayload("DND_DEMO_CELL", mDraggedEditorElement, sizeof(EditorElement*));
		ImGui::EndDragDropSource();
	}
}

void EditorLayer::ProcessDropEvent(EditorElement* target)
{
	// Assign the actor as a child to an another.
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
		{
			IM_ASSERT(payload->DataSize == sizeof(EditorElement*));
			std::vector<EditorElement*>& elements =
				EditorSceneManager::GetInstance()->GetEditorScene()->GetEditorElements();

			EditorElement* child = static_cast<EditorElement*>(mDraggedEditorElement);
			if (child->GetParent() == target)
			{
				target->RemoveChild(child);
				if (target->GetParent())
					target->GetParent()->AddChild(child);
				child->SetHierarchyLevel(target->GetHierarchyLevel());
				SetHierarchyLvRecurse(child, -1);
			}
			else
			{
				if (child->GetParent())
				{
					std::vector<Actor*>& children = child->GetParent()->GetChildActors();
					auto				 iter	  = std::find(children.begin(), children.end(), child);
					children.erase(iter);
					child->SetParent(nullptr);
				}
				target->AddChild(child);
				child->SetHierarchyLevel(target->GetHierarchyLevel() + 1);
				SetHierarchyLvRecurse(child, 1);

				//// Move "child" actor to the back of its parent.
				// elements.erase(elements.begin() + payload_n);

				//// iter = the location of parent element.
				// auto iter = std::find(elements.begin(), elements.end(), target);
				//++iter;
				// elements.insert(iter, child);

				// iter		  = std::find(elements.begin(), elements.end(), target);
				// size_t offset = iter - elements.begin();
				//// Iterate over the children of "child" Actor.
				// for (size_t i = 0; i < child->GetChildActors().size(); ++i)
				//{
				//	EditorElement* subChild = static_cast<EditorElement*>(child->GetChildActors().at(i));
				//	subChild->SetHierarchyLevel(child->GetHierarchyLevel() + 1);

				//	// Find the location of subChild in the vector.
				//	auto subIter = std::find(elements.begin(), elements.end(), subChild);
				//	elements.erase(subIter);

				//	// Insert the subChild at the back of "child"
				//	elements.insert(elements.begin() + offset, subChild);
				//}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void EditorLayer::SetHierarchyLvRecurse(EditorElement* element, int val)
{
	for (Actor* actor : element->GetChildActors())
	{
		EditorElement* subChild = static_cast<EditorElement*>(actor);
		subChild->SetHierarchyLevel(subChild->GetHierarchyLevel() + val);
		SetHierarchyLvRecurse(subChild, val);
	}
}

void EditorLayer::DisplayResourceMenu()
{
	std::string menuID = "Resource Manager";
	ImGui::Begin(menuID.c_str());
	EditorResourceManager::GetInstance()->UpdateUI();
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
	if (0 < scene->GetEditorElements().size())
	{
		if (mFocusedEditorElement)
		{
			mFocusedEditorElement->UpdateUI(false);
			if (mDeleteKeyPressed)
			{
				// Delete game object, and erase the pointed from std::vector
				ActorGroup group = mFocusedEditorElement->GetActorGroup();

				std::vector<EditorElement*>::iterator iter =
					std::find(
						scene->GetEditorElements().begin(),
						scene->GetEditorElements().end(),
						mFocusedEditorElement);
				scene->GetEditorElements().erase(iter);
				if (0 < mActorNameIdx)
					mActorNameIdx = scene->GetEditorElements().size() - 1;
			}
		}
	}
	// ResizeUIWindow(menuID);
	ImGui::End();
}

bool EditorLayer::SceneViewportSizeChanged(ImVec2 size)
{
	return size != ImGui::GetContentRegionAvail();
}

bool EditorLayer::ProjectExists(std::string projDir)
{
	return std::filesystem::exists(projDir + "//Assets") && std::filesystem::exists(projDir + "//Builds") && std::filesystem::exists(projDir + "//Chunks") && std::filesystem::exists(projDir + "//FoxtrotEngine");
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
				-> void {

				FTDS::String name = mFocusedEditorElement->GetName();
				name.Append(FileTypes::PREMADE);

				FTResourceDef resDef{
					name.C_Str(), 
					ResourceManager::GetInstance()->GetPathToAsset().C_Str() 
				};

				FTPremade* newPremade = DBG_NEW FTPremade(resDef);
				newPremade->Create(mFocusedEditorElement);
				ImGui::CloseCurrentPopup();
				mInfoType = InfoType::None;
			};
			if (mFocusedEditorElement)
			{
				FTDS::String msg;
				msg.Assign("Create Premade with name : ", mFocusedEditorElement->GetName().C_Str(), "?");
				PopUpInfo("Create Premade", msg.C_Str(), onConfirm);
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
				"Foxtrot Engine project\nalready exists in this directory.");
			break;

		case ErrorType::ProjectNotValid:
			PopUpError(
				"Project Not Valid",
				"The directory is not a valid Foxtrot Engine Project");
			break;

		case ErrorType::ProjectPathNotEmpty:
			PopUpError(
				"Folder Not Empty",
				"The directory is not empty to create a FT Engine Project.");
			break;

		case ErrorType::ChunkNotSaved:
		{
			std::function<void()> onConfirm = [this]()
				-> void {
				FTCoreEditor::GetInstance()->SetIsRunning(false);
				mErrorType = ErrorType::None;
				ImGui::CloseCurrentPopup();
			};

			std::function<void()> onCancel = [this]()
				-> void {
				mErrorType = ErrorType::None;
				ImGui::CloseCurrentPopup();
			};

			PopUpInquiry(
				"Chunk is not saved",
				".Chunk is not saved.\n Discard the chunk?",
				onConfirm,
				onCancel);
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

void EditorLayer::CreateNewProject(std::string& path)
{
	bool projExists	 = ProjectExists(path.c_str());
	bool pathIsEmpty = std::filesystem::is_empty(path);

	if (!projExists && pathIsEmpty)
	{
		PATH_PROJECT.Assign(path.c_str());

		FTDS::String&& assetDir(std::move(PATH_PROJECT));
		FTDS::String&& buildDir(std::move(PATH_PROJECT));
		FTDS::String&& chunkDir(std::move(PATH_PROJECT));
		FTDS::String&& engineDir(std::move(PATH_PROJECT));
		FTDS::String&& gameDataDir("");

		assetDir.Append("\\Assets");
		buildDir.Append("\\Builds");
		chunkDir.Append("\\Chunks");
		engineDir.Append("\\FoxtrotEngine");
		gameDataDir.Assign(engineDir.C_Str(), "\\GameData", FileTypes::GDPACK);

		std::filesystem::create_directory(assetDir.C_Str());
		std::filesystem::create_directory(buildDir.C_Str());
		std::filesystem::create_directory(chunkDir.C_Str());
		std::filesystem::create_directory(engineDir.C_Str());

		std::ofstream ofs(engineDir.C_Str());
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::GAME_DATA);
		FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CHUNK_LIST);
		FileIOHelper::EndDataPackSave(ofs, ChunkKey::CHUNK_LIST);
		FileIOHelper::EndDataPackSave(ofs, ChunkKey::GAME_DATA);
		FileIOHelper::SaveBufferToFile(ofs);
	}
	else
	{
		if (projExists)
			mErrorType = ErrorType::ProjectPathExists;
		else if (!pathIsEmpty)
			mErrorType = ErrorType::ProjectPathNotEmpty;
	}
	EditorResourceManager::GetInstance()->DeleteAll();
}

void EditorLayer::OpenProject(std::string& path)
{
	if (ProjectExists(path))
	{
		EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();
		DebugShapes::GetInstance()->DeleteAll();
		EditorResourceManager::GetInstance()->DeleteAll();
		PATH_PROJECT.Assign(path.c_str());
		ResourceManager::GetInstance()->SetPathToAsset(std::move(PATH_PROJECT));
		EditorResourceManager::GetInstance()->LoadAllResourcesInAsset();
		// EditorResourceManager::GetInstance()->Initialize(FTCoreEditor::GetInstance()->GetGameRenderer());
	}
	else
		mErrorType = ErrorType::ProjectNotValid;
}

void EditorLayer::Save(std::string& path)
{
	PATH_CHUNK.Assign(path.c_str());
	EditorChunkLoader::GetInstance()->SaveChunk(PATH_CHUNK);
	mInfoType = InfoType::ChunkIsSaved;
	SET_CHUNK_IS_SAVED(true)
}

void EditorLayer::SaveAs(std::string& path)
{
	PATH_CHUNK.Assign(path.c_str());
	EditorChunkLoader::GetInstance()->SaveChunk(PATH_CHUNK);
	mInfoType = InfoType::ChunkIsSaved;
	SET_CHUNK_IS_SAVED(true)
}

void EditorLayer::Open(std::string& path)
{
	mFocusedEditorElement = nullptr;
	EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();
	PATH_CHUNK.Assign(path.c_str());
	EditorChunkLoader::GetInstance()->LoadChunk(PATH_CHUNK);
	// LightManager::GetInstance()->Reset(FTCoreEditor::GetInstance()->GetGameRenderer());
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
	: mActorNameIdx(0)
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
	, mDraggedEditorElement(nullptr)
	, mInfoType(InfoType::None)
	, mFileMenuEvent(FileMenuEvents::None)
	, mErrorType(ErrorType::None)
{
}

EditorLayer::~EditorLayer()
{
}