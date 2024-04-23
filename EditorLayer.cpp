#ifdef _DEBUG
#include "EditorLayer.h"
#include "EditorElement.h"
#include "EditorCamera2D.h"
#include "ChunkLoader.h"
#include "Actor.h"
#include "SceneManager.h"
#include "Scene.h"
#include "ActorGroup.h"
#include "PanelUI.h"
#include "CCore.h"
#include "Ship.h"
#include "Transform.h"
#include "GroundObject.h"
#include "BackgroundActor.h"
#include "CommandHistory.h"
#include "Command.h"
#include "Rigidbody2DComponent.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

void EditorLayer::Update(float deltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	//ImGui_ImplSDLRenderer2_NewFrame();
	//ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	mSaveKeyPressed    = ImGui::IsKeyChordPressed(ImGuiModFlags_::ImGuiModFlags_Ctrl | ImGuiKey::ImGuiKey_S);
	mSaveAsKeyPressed  = ImGui::IsKeyChordPressed(ImGuiModFlags_::ImGuiModFlags_Ctrl | ImGuiModFlags_::ImGuiModFlags_Shift | ImGuiKey::ImGuiKey_S);
	mOpenKeyPressed	   = ImGui::IsKeyChordPressed(ImGuiModFlags_::ImGuiModFlags_Ctrl | ImGuiKey::ImGuiKey_O);
	mConfirmKeyPressed = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter);
	mUndoKeyPressed	   = ImGui::IsKeyChordPressed(ImGuiModFlags_::ImGuiModFlags_Ctrl | ImGuiKey::ImGuiKey_Z);
	mRedoKeyPressed    = ImGui::IsKeyChordPressed(ImGuiModFlags_::ImGuiModFlags_Ctrl | ImGuiModFlags_::ImGuiModFlags_Shift | ImGuiKey::ImGuiKey_Z);

	CommandHistory::GetInstance()->Update();

	DisplayFileMenu();
	DisplayHierarchyMenu();
	EditorCamera2D::GetInstance()->DisplayCameraMenu();

	for (int i = 0; i < mEditorElements.size(); ++i)
	{
		//mEditorElements[i]->EditorUpdateComponents(deltaTime);
		if (mEditorElements[i]->GetIsFocused())
		{
			mEditorElements[i]->UIUpdate();
		}
	}
	ApplyCommandHistory();
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

void EditorLayer::DisplayFileMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		std::string selection = {};
		if (ImGui::Button("File"))
		{
			ImGui::OpenPopup("FilePopUp");
		}

		if (ImGui::BeginPopup("FilePopUp"))
		{
			const size_t maxMenuEle = 3;
			const std::string fileMenu[maxMenuEle] = { "Save", "Save As", "Open" };
			for (size_t i = 0; i < maxMenuEle; ++i)
				if (ImGui::Selectable(fileMenu[i].c_str()))
					selection = fileMenu[i];
			ImGui::EndPopup();
		}

		if (selection == "Save" || mSaveKeyPressed)
		{
			if (mCurrFileSaved)
			{
				if (!mCurrFilePathName.empty())
					ChunkLoader::GetInstance()->SaveChunk(mCurrFilePathName);
				else
					LogString("Saved file path doesn't exist but trying to access it");
			}
			else
			{
				IGFD::FileDialogConfig config;
				config.path = ".";
				config.countSelectionMax = 1;
				config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
				ImGuiFileDialog::Instance()->OpenDialog("SaveChunkFile", "Save", CHUNK_FORMAT, config);
			}
		}
		else if (selection == "Save As" || mSaveAsKeyPressed)
		{
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.countSelectionMax = 1;
			config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
			ImGuiFileDialog::Instance()->OpenDialog("SaveChunkFile", "Save As", CHUNK_FORMAT, config);
		}
		else if (selection == "Open" || mOpenKeyPressed)
		{
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.countSelectionMax = 1;
			ImGuiFileDialog::Instance()->OpenDialog("OpenChunkFile", "Open Chunk", CHUNK_FORMAT, config);
		}


		if (ImGui::Button("New Game Object"))
		{
			int size = EditorLayer::GetInstance()->GetEditorElements().size();
			Scene* currScene = SceneManager::GetInstance()->GetCurrScene();
			Ship* ship = new Ship(currScene);
			ship->SetName(L"Game Object" + std::to_wstring(size));
			AddEditorElement(ship);
		}
		if (ImGui::Button("Play"))
		{
			if (mCurrFileSaved)
			{
				if (!mCurrFilePathName.empty())
				{
					ChunkLoader::GetInstance()->SaveChunk(mCurrFilePathName);
					EditorLayer::GetInstance()->GetEditorElements().clear();
					ChunkLoader::GetInstance()->LoadChunk(mCurrFilePathName);
					CCore::GetInstance()->SetIsUpdatingGame(true);
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
				if (!mCurrFilePathName.empty())
				{
					CCore::GetInstance()->SetIsUpdatingGame(false);
					SceneManager::GetInstance()->GetCurrScene()->DeleteAll();
					EditorLayer::GetInstance()->GetEditorElements().clear();
					ChunkLoader::GetInstance()->LoadChunkToEditor(mCurrFilePathName);
				}
			}
		}
		if (ImGuiFileDialog::Instance()->Display("SaveChunkFile"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				mCurrFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				ChunkLoader::GetInstance()->SaveChunk(mCurrFilePathName);
				mCurrFileSaved = true;
			}
			ImGuiFileDialog::Instance()->Close();
		}

		if (ImGuiFileDialog::Instance()->Display("OpenChunkFile"))
		{
			if (ImGuiFileDialog::Instance()->IsOk())
			{
				EditorLayer::GetInstance()->GetEditorElements().clear();
				mCurrFilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				ChunkLoader::GetInstance()->LoadChunkToEditor(mCurrFilePathName);
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
	ImGui::Begin("Hierarchy Menu");

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

void EditorLayer::ShutDown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void EditorLayer::AddEditorElement(Actor* actor)
{
	UnfocusEditorElements();
	EditorElement* editorElement = new EditorElement(actor);
	editorElement->SetIsFocused(true);
	mEditorElements.emplace_back(editorElement);
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
	, mCurrFilePathName{}
	, mHierarchyIdx(0)
	, mActorNameIdx(0)
	, mSaveKeyPressed(false)
	, mSaveAsKeyPressed(false)
	, mOpenKeyPressed(false)
	, mUndoKeyPressed(false)
	, mRedoKeyPressed(false)
	, mConfirmKeyPressed(false)
{
	// Initial command stored in front of every following commands
	CommandHistory::GetInstance()->AddCommand(new IntEditCommand(mActorNameIdx, 0));
}

EditorLayer::~EditorLayer()
{}
#endif