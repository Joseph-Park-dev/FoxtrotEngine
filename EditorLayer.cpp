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
#include "FTCore.h"
#include "FTCoreEditor.h"
#include "Ship.h"
#include "Transform.h"
#include "GroundObject.h"
#include "BackgroundActor.h"
#include "CommandHistory.h"
#include "Command.h"
#include "Rigidbody2DComponent.h"
#include "FoxtrotRenderer.h"
#include "RenderTextureClass.h"
#include "KeyInputManager.h"

#include <unordered_map>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
#include <d3d11.h>

void EditorLayer::Update(float deltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();
	mSaveKeyPressed    = ImGui::IsKeyChordPressed(ImGuiModFlags_::ImGuiModFlags_Ctrl | ImGuiKey::ImGuiKey_S);
	mSaveAsKeyPressed  = ImGui::IsKeyChordPressed(ImGuiModFlags_::ImGuiModFlags_Ctrl | ImGuiModFlags_::ImGuiModFlags_Shift | ImGuiKey::ImGuiKey_S);
	mOpenKeyPressed	   = ImGui::IsKeyChordPressed(ImGuiModFlags_::ImGuiModFlags_Ctrl | ImGuiKey::ImGuiKey_O);
	mConfirmKeyPressed = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Enter);
	mUndoKeyPressed	   = ImGui::IsKeyChordPressed(ImGuiModFlags_::ImGuiModFlags_Ctrl | ImGuiKey::ImGuiKey_Z);
	mRedoKeyPressed    = ImGui::IsKeyChordPressed(ImGuiModFlags_::ImGuiModFlags_Ctrl | ImGuiModFlags_::ImGuiModFlags_Shift | ImGuiKey::ImGuiKey_Z);
	mDeleteKeyPressed  = ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Delete);

	CommandHistory::GetInstance()->Update();
	
	DisplayViewport();
	DisplayFileMenu();
	DisplayHierarchyMenu();
	DisplayInspectorMenu();
	EditorCamera2D::GetInstance()->DisplayCameraMenu();
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

void EditorLayer::UpdateSceneViewportArea()
{
	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();
	vMin.x += ImGui::GetWindowPos().x;
	vMin.y += ImGui::GetWindowPos().y;
	vMax.x += ImGui::GetWindowPos().x;
	vMax.y += ImGui::GetWindowPos().y;
	mSceneViewportPos = vMin;
	mSceneViewportSize = vMax - vMin;
}

void EditorLayer::DisplayViewport()
{
	ImGui::Begin("Scene");
	UpdateSceneViewportArea();
	FoxtrotRenderer* renderer = FTCoreEditor::GetInstance()->GetGameRenderer();
	ID3D11ShaderResourceView* viewportTexture = renderer->GetRenderTexture()->GetShaderResourceView().Get();
	ImGui::Image((void*)viewportTexture, mSceneViewportSize);
	ImGui::End();
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
			Scene* currScene = SceneManager::GetInstance()->GetCurrScene();
			AddEditorElement(currScene);
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
				if (!mCurrFilePathName.empty())
				{
					FTCoreEditor::GetInstance()->SetIsUpdatingGame(false);
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

void EditorLayer::DisplayInspectorMenu()
{
	std::string menuID = "Inspector";
	ImGui::Begin(menuID.c_str());
	for (int i = 0; i < mEditorElements.size(); ++i)
	{
		//mEditorElements[i]->EditorUpdateComponents(deltaTime);
		if (mEditorElements[i]->GetIsFocused())
		{
			mEditorElements[i]->UIUpdate();
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

void EditorLayer::AddEditorElement(Scene* scene)
{
	UnfocusEditorElements();
	EditorElement* editorElement = new EditorElement(scene);
	editorElement->SetName(L"Game Object" + std::to_wstring(mEditorElements.size()));
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
	, mDeleteKeyPressed(false)
{
	// Initial command stored in front of every following commands
	CommandHistory::GetInstance()->AddCommand(new IntEditCommand(mActorNameIdx, 0));
}

EditorLayer::~EditorLayer()
{}
#endif