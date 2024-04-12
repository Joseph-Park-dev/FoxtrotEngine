#ifdef _DEBUG
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"

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

void EditorLayer::Init(SDL_Window* window, SDL_Renderer* renderer)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer2_Init(renderer);
}

void EditorLayer::ProcessInput(SDL_Event* event)
{
	ImGui_ImplSDL2_ProcessEvent(event);
}

void EditorLayer::Update(float deltaTime)
{
	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
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

void EditorLayer::Render(SDL_Renderer* renderer)
{
	ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

void EditorLayer::DisplayEditorElements(SDL_Renderer* renderer)
{
	for (int i = 0; i < mEditorElements.size(); ++i)
		mEditorElements[i]->Render(renderer);
}

void EditorLayer::DisplayFileMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::Button("Save") || mSaveKeyPressed)
		{
			if (mCurrFileSaved)
			{
				if (!mCurrFilePathName.empty())
					ChunkLoader::GetInstance()->SaveChunk(mCurrFilePathName);
				else
					SDL_Log("Saved file path doesn't exist but trying to access it");
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
		if (ImGui::Button("Save As") || mSaveAsKeyPressed)
		{
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.countSelectionMax = 1;
			config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
			ImGuiFileDialog::Instance()->OpenDialog("SaveChunkFile", "Save As", CHUNK_FORMAT, config);
		}
		if (ImGui::Button("Open") || mOpenKeyPressed)
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
					SDL_Log("Saved file path doesn't exist but trying to access it");
			}
			else
				SDL_Log("Chunk file must be saved before playing");
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
		for (Uint32 i = 0; i < mEditorElements.size(); ++i)
		{
			actorNames[i] = ToString(mEditorElements[i]->GetName());
		}
		bool* selection = new bool[mEditorElements.size()];
		for (Uint32 i = 0; i < mEditorElements.size(); ++i)
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
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
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