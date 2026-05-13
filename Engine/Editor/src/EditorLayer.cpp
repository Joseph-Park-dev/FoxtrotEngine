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
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_dx11.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/imgui_internal.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <d3d11.h>

#include "EditorElement.h"
#include "EditorChunkLoader.h"
#include "CommandHistory.h"
#include "Command.h"
#include "ViewportRenderer.h"
#include "EditorSceneManager.h"
#include "EditorScene.h"
#include "EditorCamera.h"
#include "Manager/EditorShapes.h"
#include "Manager/ResourceManager.h"
#include "ResourceSystem/FTResource.h"

#include "Entity/D3D11Window.h"
#include "InputSystem/D3D11InputDevice.h"
#include "EditorRenderer.h"
#include "EventSystem/EventFunctions.h"
#include "FileSystem/FileTypes.h"

#include "ResourceSystem/ResPath.h"
#include "Plugin/GetFunc.h"
#include "Manager/AnimationManager.h"
#include "Engine.h"

namespace Editor
{
	using namespace Core;

	void EditorLayer::Initialize(Editor::EditorRenderer* renderer)
	{
		mRenderer = renderer;
		LoadEditorConfig();

		mGetProjPathFunc  = GetFunc<GET_PROJ_PATH_FUNC>(DLLPaths::CORE_EDITOR, ProcName::GetProjectPath);
		mGetChunkPathFunc = GetFunc<GET_CHUNK_PATH_FUNC>(DLLPaths::CORE_EDITOR, ProcName::GetChunkPath);
		mGetAssetPathFunc = GetFunc<GET_ASSET_PATH_FUNC>(DLLPaths::CORE_EDITOR, ProcName::GetAssetPath);
		mSetChunkIsSaved = GetFunc<SET_CHUNK_IS_SAVED_FUNC>(DLLPaths::CORE_EDITOR, ProcName::SetChunkIsSaved);
	}

	void EditorLayer::Update(float deltaTime, D3D11::D3D11Window* editorWin, D3D11::D3D11InputDevice* input, Editor::EditorRenderer* renderer)
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
		DisplayInspectorMenu();

		ImGui::Begin("Camera Menu");
		EditorCamera::GetInstance()->DisplayMainCameraMenu();
		EditorCamera::GetInstance()->DisplayEditorCameraMenu();
		ImGui::End();

		DisplayInfoMessage();
		DisplayErrorMessage();

		DisplayViewport(editorWin, input, renderer);
		DisplayFrameRate();
		ImGui::EndFrame();
	}

	void EditorLayer::DisplayViewport(D3D11::D3D11Window* editorWin, D3D11::D3D11InputDevice* input, Editor::EditorRenderer* renderer)
	{
		ImGui::Begin("Scene");
		if (ImGui::IsWindowFocused())
			mCursorOnViewport = true;
		else
			mCursorOnViewport = false;

		ImVec2 windowPos  = ImGui::GetWindowPos() + ImGui::GetWindowContentRegionMin();
		ImVec2 contentReg = ImGui::GetContentRegionAvail();

		ImVec2 size(editorWin->GetRenderArea()->GetSize().x, editorWin->GetRenderArea()->GetSize().y);
		if (input->MOUSE_HOLD(D3D11::MOUSE::MOUSE_LEFT) && SceneViewportSizeChanged(size))
		{
			mIsResizingViewport = true;
		}
		if (mIsResizingViewport && input->MOUSE_AWAY(D3D11::MOUSE::MOUSE_LEFT))
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

	void EditorLayer::AdjustGUI(bool* opened)
	{
		if (!ImGui::Begin("GUI Preferences", opened))
			ImGui::End();
		else
		{
			ImGuiIO& io = ImGui::GetIO();
			CommandHistory::GetInstance()->UpdateFloatValue("Font Scale", io.FontGlobalScale);
		}
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
				if (Core::CHUNK_IS_SAVED)
				{
					if (!mGetProjPathFunc()->IsEmpty())
					{
						EditorChunkLoader::GetInstance()->SaveChunk(*mGetChunkPathFunc());
						printf("Chunk saved to %s", mGetChunkPathFunc()->C_Str());
						mInfoType = InfoType::ChunkIsSaved;
					}
					else
						printf("Saved file path doesn't exist but trying to access it");
				}
				else
				{
					IGFD::FileDialogConfig config;
					config.path = mGetProjPathFunc()->C_Str();
					ImGuiFileDialog::Instance()->OpenDialog(fileMenu[2], "Choose Directory", Core::FileTypes::CHUNK, config);
					mFileMenuEvent = FileMenuEvents::Save;
				}
			}
			else if (selection == fileMenu[3] || mSaveAsKeyPressed)
			{
				IGFD::FileDialogConfig config;
				config.path = mGetProjPathFunc()->C_Str();
				ImGuiFileDialog::Instance()->OpenDialog(fileMenu[3], "Choose Directory", Core::FileTypes::CHUNK, config);
				mFileMenuEvent = FileMenuEvents::SaveAs;
			}
			else if (selection == fileMenu[4] || mOpenKeyPressed)
			{
				if (!mGetProjPathFunc()->IsEmpty())
				{
					IGFD::FileDialogConfig config;
					config.path = mGetProjPathFunc()->C_Str();
					ImGuiFileDialog::Instance()->OpenDialog(fileMenu[4], "Choose File", Core::FileTypes::CHUNK, config);
					mFileMenuEvent = FileMenuEvents::Open;
				}
			}

			DisplayEditMenu();
			DisplayManagersMenu();

			if (ImGui::Button("New Empty Actor"))
				reinterpret_cast<EditorScene*>(EditorSceneManager::GetInstance()->GetCurrentScene())
					->AddEditorElement();

			if (ImGui::Button("Play"))
			{
				if (Core::CHUNK_IS_SAVED)
				{
					if (!mGetChunkPathFunc()->IsEmpty())
					{
						// Clear up the scene to load the current chunk file.
						mFocusedEditorElement = nullptr;
						EditorChunkLoader::GetInstance()->SaveChunk(*mGetChunkPathFunc());
						// DebugShapes::GetInstance()->DeleteAll();
						// UIManager::GetInstance()->Reset();
						// CollisionManager::GetInstance()->Reset();
						EditorSceneManager::GetInstance()->GetCurrentScene()->DeleteAll();

						// Copy the chunk file.
						EditorChunkLoader::GetInstance()->CopyChunk(*mGetChunkPathFunc());
						Common::FTDS::String& copiedPath = EditorChunkLoader::GetInstance()->CurrentChunk();

						// Load the copied chunk file.
						EditorChunkLoader::GetInstance()->LoadChunk(copiedPath);

						// Start updating the game.
						Engine::GetInstance()->SetIsUpdating(true);
					}
					else
						printf("Saved file path doesn't exist but trying to access it");
				}
				else
					printf("Chunk file must be saved before playing");
			}
			if (ImGui::Button("Stop"))
			{
				if (Core::CHUNK_IS_SAVED)
				{
					if (!mGetChunkPathFunc()->IsEmpty())
					{
						// Delete the created copy.
						EditorChunkLoader::GetInstance()->DeleteCopiedChunk();

						// Clear up the scene to load the current chunk file.
						mFocusedEditorElement = nullptr;
						Engine::GetInstance()->SetIsUpdating(false);
						EditorShapes::GetInstance()->DeleteAll();
						// CollisionManager::GetInstance()->Reset();
						EditorSceneManager::GetInstance()->GetCurrentScene()->DeleteAll();

						// Reload current scene again.
						EditorChunkLoader::GetInstance()->LoadChunk(*mGetChunkPathFunc());
					}
				}
			}
			if (ImGui::Button("Wireframe"))
			{
				// if (mRenderer)
				//{
				//	if (mRenderer->GetFillMode() == D3D11::FillMode::SOLID)
				//	{
				//		mRenderer->SetFillMode(D3D11::FillMode::WIRE_FRAME);
				//		D3D11::ResourceManager::GetResource(D3D11::D3D11PSO::Type, )
				//	}
				//	else if (renderer->GetFillMode() == FillMode::WireFrame)
				//		renderer->SetFillMode(FillMode::Solid);
				// }
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

	void EditorLayer::DisplayEditMenu()
	{
		const size_t maxMenuEle			= 1;
		const char*	 menu[maxMenuEle]	= { "Preferences" };
		static bool	 opened[maxMenuEle] = { false };
		if (ImGui::Button("Edit"))
			ImGui::OpenPopup("EditPopUp");

		if (ImGui::BeginPopup("EditPopUp"))
		{
			for (size_t i = 0; i < maxMenuEle; ++i)
				if (ImGui::Selectable(menu[i]))
					opened[i] = menu[i];
			ImGui::EndPopup();
		}

		if (opened[0])
			AdjustGUI(&opened[0]);
	}

	void EditorLayer::DisplayManagersMenu()
	{
		const size_t maxMenuEle			= 4;
		const char*	 menu[maxMenuEle]	= { "Animation Manager", "Sound Manager", "Font Manager", "Collision Manager" };
		static bool	 opened[maxMenuEle] = { false, false, false, false };

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
			D3D11::AnimationManager::GetInstance()->UpdateUI(&opened[0]);
		// if (opened[1])
		//	SoundManager::GetInstance()->UpdateUI(&opened[1]);
		// if (opened[2])
		//	FontManager::GetInstance()->UpdateUI(&opened[2]);
		// if (opened[3])
		//	CollisionManager::GetInstance()->UpdateUI(&opened[3]);
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
				{
					ImGui::PushID(ele->GetID());
					DisplaySelection(ele, idx);
					ImGui::PopID();
				}
			}
			ImGui::EndListBox();

			EditorSceneManager::GetInstance()->GetCurrentScene()->Actors()->IterateArray([&](Core::Actor* actor) {
				EditorElement* ele = static_cast<EditorElement*>(actor);
				ele->SetIsDisplayed(false);
			});
		}

		// if (mDuplicateKeyPressed)
		//	EditorSceneManager::GetInstance()->GetEditorScene()->AddEditorElement(mFocusedEditorElement);
		ImGui::End();
	}

	void EditorLayer::DisplaySelection(EditorElement* element, size_t& index)
	{
		Common::FTDS::String indentedName = Common::FTDS::String(element->GetHierarchyLevel(), '\t');
		indentedName.Append(element->GetName());

		if (ImGui::Selectable(indentedName.C_Str(), mActorNameIdx == index))
		{
			mActorNameIdx = static_cast<int>(index);

			// IntEditCommand* command = DBG_NEW IntEditCommand(mActorNameIdx);
			// command->SetNextVal(mActorNameIdx);
			//  CommandHistory::GetInstance()->AddCommand(command);

			EditorSceneManager::GetInstance()->GetEditorScene()->UnfocusEditorElements();
			mFocusedEditorElement = element;
			mFocusedEditorElement->SetIsFocused(true);
		}

		ProcessDragEvent(element);
		ProcessDropEvent(element);
		++index;

		// Recurse to display child Actors in the list.
		if (0 < element->GetChildActors()->GetSize())
		{
			Common::FTDS::DynamicArray<Actor*>* childActors = element->GetChildActors();
			for (auto child = childActors->Begin(); child != childActors->End(); ++child)
			{
				EditorElement* childElem = static_cast<EditorElement*>(*child);
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
						Common::FTDS::DynamicArray<Actor*>* children = child->GetParent()->GetChildActors();

						int pos = children->Find(child);
						if (pos != -1)
							children->Erase(pos);
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
		Common::FTDS::DynamicArray<Core::Actor*>* childActors = element->GetChildActors();
		if (childActors->GetSize() < 1)
			return;

		for (auto child = childActors->Begin(); child != childActors->End(); ++child)
		{
			EditorElement* subChild = static_cast<EditorElement*>(*child);
			subChild->SetHierarchyLevel(subChild->GetHierarchyLevel() + val);
			SetHierarchyLvRecurse(subChild, val);
		}
	}

	void EditorLayer::DisplayResourceMenu()
	{
		std::string menuID = "Resource Manager";
		ImGui::Begin(menuID.c_str());
		//Editor::ResourceManager::GetInstance()->UpdateUI();
		ImGui::End();
	}

	void EditorLayer::DisplayInspectorMenu()
	{
		std::string menuID = "Inspector";
		ImGui::Begin(menuID.c_str());
		EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
		if (0 < scene->GetActors()->GetSize())
		{
			if (mFocusedEditorElement)
			{
				mFocusedEditorElement->UpdateUI(false);
				if (mDeleteKeyPressed)
				{
					// Delete game object, and erase the pointed from std::vector
					ActorGroup group	= mFocusedEditorElement->GetActorGroup();
					int		   actorPos = scene->Actors()->Find(mFocusedEditorElement);
					scene->Actors()->Erase(actorPos);

					delete mFocusedEditorElement;
					mFocusedEditorElement = nullptr;

					if (0 < mActorNameIdx)
						mActorNameIdx = scene->Actors()->GetSize() - 1;
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
					Common::FTDS::String name = mFocusedEditorElement->GetName();
					name.Append(FileTypes::PREMADE);

					Common::FTDS::String* path = mGetAssetPathFunc();
					path->Append(name);

					Common::FTResourceDef resDef{
						name.C_Str(),
						path->C_Str()
					};

					Core::FTPremade* newPremade = DBG_NEW Core::FTPremade(resDef, mFocusedEditorElement);
					ImGui::CloseCurrentPopup();
					mInfoType = InfoType::None;
				};
				if (mFocusedEditorElement)
				{
					Common::FTDS::String msg;
					msg.Assign("Create Premade with name : ");
					msg.Append(mFocusedEditorElement->GetName().C_Str());
					msg.Append("?");
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
					SaveEditorConfig();
					Engine::GetInstance()->SetIsRunning(false);
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
			mGetProjPathFunc()->Assign(path.c_str());

			Common::FTDS::String&& assetDir(std::move(*mGetProjPathFunc()));
			Common::FTDS::String&& buildDir(std::move(*mGetProjPathFunc()));
			Common::FTDS::String&& chunkDir(std::move(*mGetProjPathFunc()));
			Common::FTDS::String&& engineDir(std::move(*mGetProjPathFunc()));
			Common::FTDS::String&& gameDataDir("");

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
			Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::GAME_DATA);
			Common::FileIOHelper::BeginDataPackSave(ofs, ChunkKey::CHUNK_LIST);
			Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::CHUNK_LIST);
			Common::FileIOHelper::EndDataPackSave(ofs, ChunkKey::GAME_DATA);
			Common::FileIOHelper::SaveBufferToFile(ofs);
		}
		else
		{
			if (projExists)
				mErrorType = ErrorType::ProjectPathExists;
			else if (!pathIsEmpty)
				mErrorType = ErrorType::ProjectPathNotEmpty;
		}
	}

	void EditorLayer::OpenProject(std::string& path)
	{
		if (ProjectExists(path))
		{
			EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();
			EditorShapes::GetInstance()->DeleteAll();
			//ResourceManager::GetInstance()->DeleteAll();
			mGetProjPathFunc()->Assign(path.c_str());
			//Editor::ResourceManager::GetInstance()->LoadAllResourcesInAsset();
			// EditorResourceManager::GetInstance()->Initialize(FTCoreEditor::GetInstance()->GetGameRenderer());
		}
		else
			mErrorType = ErrorType::ProjectNotValid;
	}

	void EditorLayer::Save(std::string& path)
	{
		mGetChunkPathFunc()->Assign(path.c_str());
		EditorChunkLoader::GetInstance()->SaveChunk(*mGetChunkPathFunc());
		mInfoType = InfoType::ChunkIsSaved;
		mSetChunkIsSaved(true);
	}

	void EditorLayer::SaveAs(std::string& path)
	{
		mGetChunkPathFunc()->Assign(path.c_str());
		EditorChunkLoader::GetInstance()->SaveChunk(*mGetChunkPathFunc());
		mInfoType = InfoType::ChunkIsSaved;
		mSetChunkIsSaved(true);
	}

	void EditorLayer::Open(std::string& path)
	{
		mFocusedEditorElement = nullptr;
		EditorSceneManager::GetInstance()->GetEditorScene()->DeleteAll();
		mGetChunkPathFunc()->Assign(path.c_str());
		EditorChunkLoader::GetInstance()->LoadChunk(*mGetChunkPathFunc());
		// LightManager::GetInstance()->Reset(FTCoreEditor::GetInstance()->GetGameRenderer());
		mSetChunkIsSaved(true);
	}

	void EditorLayer::SaveEditorConfig()
	{
		ImGuiIO&	  io = ImGui::GetIO();
		std::ofstream ofs(Editor::Path::EDITOR_CONFIG);
		Common::FileIOHelper::BeginDataPackSave(ofs, ConfigKey::GUI);
		Common::FileIOHelper::SaveFloat(ofs, ConfigKey::FONT_SCALE, io.FontGlobalScale);
		Common::FileIOHelper::EndDataPackSave(ofs, ConfigKey::GUI);
		Common::FileIOHelper::SaveBufferToFile(ofs);
	}

	void EditorLayer::LoadEditorConfig()
	{
		ImGuiIO&	  io = ImGui::GetIO();
		std::ifstream ifs(Path::EDITOR_CONFIG);
		if (!ifs.good())
			SaveEditorConfig();

		Common::FileIOHelper::BeginDataPackLoad(ifs, ConfigKey::GUI);
		Common::FileIOHelper::LoadFloat(ifs, io.FontGlobalScale);
	}

	void EditorLayer::Render()
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
		: mRenderer(nullptr)
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
		, mDraggedEditorElement(nullptr)
		, mInfoType(InfoType::None)
		, mFileMenuEvent(FileMenuEvents::None)
		, mErrorType(ErrorType::None)
	{
	}

	EditorLayer::~EditorLayer()
	{
	}
} // namespace Editor