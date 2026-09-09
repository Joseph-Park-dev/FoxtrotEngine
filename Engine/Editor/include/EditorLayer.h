// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton object which is responsible of UIs in Foxtrot Editor.
/// All Imgui UIs' Update & Render operations are processed here.
/// </summary>

#pragma once

#include <functional>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include "Utility/SingletonMacro.h"
#include "Manager/DirectoryHelper.h"
#include "EditorElement.h"
#include "CommandHistory.h"

namespace Graphics
{
	class IWindow;
	class IRenderer;
	class ICamera;
}

namespace InputSystem
{
	class IInputDevice;
}

namespace D3D11
{
	class D3D11Window;
	class D3D11InputDevice;
	class ViewportRenderer;
} // namespace D3D11

namespace Editor
{
	enum class ErrorType
	{
		None,
		ProjectPathExists,
		ProjectPathNotEmpty,
		ChunkNotSaved,
		ProjectNotValid
	};

	enum class InfoType
	{
		None,
		ChunkIsSaved,
		PremadeIsCreated
	};

	enum class FileMenuEvents
	{
		None,
		NewProject,
		OpenProject,
		Save,
		SaveAs,
		Open
	};

	class Command;
	class EditorCamera;
	class EditorLayer
	{
		SINGLETON(EditorLayer)
	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param renderer Renderer providing the graphics device and current render state.
		void Initialize(Graphics::IRenderer* renderer);
        /// @brief Updates the viewport renderer used by subsequent operations.
        /// @param viewport Replacement viewport renderer.
        void SetViewportRenderer(D3D11::ViewportRenderer* viewport) { mViewport = viewport; }
		/// @brief Advances frame-dependent state using the current time step.
		/// @param deltaTime Elapsed frame time in seconds.
		/// @param editorWin Editor window receiving the UI or rendering output.
		/// @param input Input state for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param gameCam Camera used to render the game scene.
		/// @param editorCam Camera used for editor navigation.
		void Update(
			float deltaTime, 
			Graphics::IWindow* editorWin, 
			InputSystem::IInputDevice* input, 
			Graphics::IRenderer* renderer,
			Graphics::ICamera* gameCam, 
			Editor::EditorCamera* editorCam);
		/// @brief Submits this object's graphics work for the current frame.
		void Render();
		/// @brief Releases runtime services and resources during engine shutdown.
		void ShutDown();

	public:
		/// @brief Exposes the editor element currently selected for inspection.
		/// @return Borrowed access to the focused editor element.
		EditorElement* FocusedEditorElement() { return mFocusedEditorElement; }
		/// @brief Returns the actor name idx used by this editor layer.
		/// @return Borrowed access to the actor name idx.
		/// @note Changes through the returned reference affect this object's stored state.
		int&		   GetActorNameIdx() { return mActorNameIdx; }
		/// @brief Returns the undo key pressed used by this editor layer.
		/// @return Current value of the undo key pressed flag.
		bool		   GetUndoKeyPressed() const { return mUndoKeyPressed; }
		/// @brief Returns the redo key pressed used by this editor layer.
		/// @return Current value of the redo key pressed flag.
		bool		   GetRedoKeyPressed() const { return mRedoKeyPressed; }
		/// @brief Returns the confirm key pressed used by this editor layer.
		/// @return Current value of the confirm key pressed flag.
		bool		   GetConfirmKeyPressed() const { return mConfirmKeyPressed; }
		/// @brief Returns the error type used by this editor layer.
		/// @return Current error type.
		ErrorType	   GetErrorType() const { return mErrorType; }

		// ImGuiFileDialogFlags& GetFileSelectFlag() { return mFileSelectFlag; }

		/// @brief Updates the info type used by subsequent operations.
		/// @param type Replacement info type.
		void SetInfoType(InfoType type) { mInfoType = type; }
		/// @brief Updates the error type used by subsequent operations.
		/// @param type Replacement error type.
		void SetErrorType(ErrorType type) { mErrorType = type; }

		// Is the cursor on the viewport area?
		/// @brief Tests whether the cursor lies inside the scene viewport bounds.
		/// @return True when the cursor lies inside the scene viewport bounds; otherwise false.
		bool CursorOnViewport() const;

	public:
		// Pops up a message box with confirm button only.
		/// @brief Shows an informational popup.
		/// @param title Window title or dialog caption.
		/// @param msg Windows message containing input or window data.
		/// @param onConfirm Callback invoked when the operation is confirmed.
		template <class FUNCTOR>
		void PopUpInfo(const char* title, const char* msg, FUNCTOR&& onConfirm)
		{
			ImGui::OpenPopup(title);
			// Always center this window when appearing
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal(title, NULL))
			{
				ImGui::Text(msg);
				ImGui::Separator();
				ImGui::SetItemDefaultFocus();

				if (ImGui::Button("Confirm", ImVec2(120, 0)))
				{
					onConfirm();
					ImGui::CloseCurrentPopup();
					mInfoType = InfoType::None;
				}
				if (ImGui::Button("Cancel", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
					mInfoType = InfoType::None;
				}
				ImGui::EndPopup();
			}
		}

		// Pops up a message box with confirm and cancel buttons.
		// (User needs to choose)
		/// @brief Shows a confirmation popup and reports the user's choice.
		/// @param title Window title or dialog caption.
		/// @param msg Windows message containing input or window data.
		/// @param onConfirm Callback invoked when the operation is confirmed.
		/// @param onCancel Callback invoked when the operation is cancelled.
		template <class FUNCTOR>
		void PopUpInquiry(const char* title, const char* msg, FUNCTOR&& onConfirm, FUNCTOR&& onCancel)
		{
			ImGui::OpenPopup(title);
			// Always center this window when appearing
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal(title, NULL))
			{
				ImGui::Text(msg);
				ImGui::Separator();
				ImGui::SetItemDefaultFocus();

				if (ImGui::Button("Confirm", ImVec2(120, 0)))
					onConfirm();

				if (ImGui::Button("Cancel", ImVec2(120, 0)))
					onCancel();
				ImGui::EndPopup();
			}
		}

	private:
		Core::GET_PROJ_PATH_FUNC	  mGetProjPathFunc;
		Core::GET_CHUNK_PATH_FUNC	  mGetChunkPathFunc;
		Core::GET_ASSET_PATH_FUNC	  mGetAssetPathFunc;
		Core::SET_CHUNK_IS_SAVED_FUNC mSetChunkIsSavedFunc;

		int mActorNameIdx;

		bool mSaveKeyPressed;
		bool mSaveAsKeyPressed;
		bool mOpenKeyPressed;
		bool mConfirmKeyPressed;

		bool mUndoKeyPressed;
		bool mRedoKeyPressed;
		bool mDeleteKeyPressed;
		bool mDuplicateKeyPressed;

		// Copy+Paste related
		EditorElement* mFocusedEditorElement;
		void*		   mDraggedEditorElement;

		// Viewport related
		bool mIsResizingViewport;
		bool mCursorOnViewport;

		// UI event types
		InfoType	   mInfoType;
		ErrorType	   mErrorType;
		FileMenuEvents mFileMenuEvent;

		D3D11::ViewportRenderer* mViewport;

	private:
		/// <summary>
		//  Renders viewport.
		//  This is execptionally placed in Update() due to its requirement
		//  to be nested in ImGUi's Frame.
		/// @brief Displays the rendered scene texture and handles viewport interaction.
		/// @param editorWin Editor window receiving the UI or rendering output.
		/// @param input Input state for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		void DisplayViewport(Graphics::IWindow* editorWin, InputSystem::IInputDevice* input, Graphics::IRenderer* renderer);

		// Displays current frame rate.
		/// @brief Displays current timing and frame-rate information in the editor.
		void DisplayFrameRate();

		/// @brief Repositions or sizes the current ImGui control to fit the available content region.
		/// Change the GUI values such as font size.
		/// @param opened Whether the editor window remains open.
		void AdjustGUI(bool* opened);

		// Displays menu docked at the top of the screen
		// & calls the related functions.
		/// @brief Builds the editor's top-level menu bar.
		void DisplayMainMenuBar();

		/// @brief Builds the editor's edit menu, including command-history actions.
		/// Displays Edit menu for project-wise settings.
		void DisplayEditMenu();

		// Displays menu docked at the top of the screen
		// & calls the related functions.
		/// @brief Builds editor controls for engine manager state.
		void DisplayManagersMenu();

		// Displays list of EditorElements in the Scene.
		// & calls the related functions.
		/// @brief Draws the scene actor hierarchy and processes selection changes.
		void DisplayHierarchyMenu();

		// Displays a single selection in the hierarchy.
		/// @brief Draws selectable entries and applies the selected value.
		/// @param actor Actor participating in this operation.
		/// @param index Zero-based element index.
		void DisplaySelection(EditorElement* actor, size_t& index);

		/// @brief Handles editor drag interactions for the selected object.
		/// @param from Source container whose values are copied.
		void ProcessDragEvent(EditorElement* from);
		/// @brief Handles an item dropped onto the editor target.
		/// @param target Target object or value sought by the operation.
		void ProcessDropEvent(EditorElement* target);

		// Recurse through the children of an element, add "val"
		// to the hierarchy levels.
		/// @brief Updates the hierarchy lv recurse used by subsequent operations.
		/// @param element Element to insert or edit.
		/// @param val Value to assign, insert, or process.
		void SetHierarchyLvRecurse(EditorElement* element, int val);

		// Displays FTResources loaded to current project.
		// & calls the related functions.
		/// @brief Draws the editor's resource management panel.
		void DisplayResourceMenu();

		// Displays the focused EditorElement's info.
		// & calls the related functions.
		/// @brief Draws editable properties for the currently selected editor element.
		void DisplayInspectorMenu();

		// Viewport size is changed by dragging.
		/// @brief Updates dependent camera and rendering state after the scene viewport is resized.
		/// @param size Number of elements or bytes required by the operation.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		bool SceneViewportSizeChanged(ImVec2 size);

		// Does Foxtrot Project exists in projDir?
		// std::string is used exceptionally because of readability issue.
		/// @brief Tests whether the required project files or directory are present.
		/// @param projDir Project directory.
		/// @return True when the required project files or directory are present; otherwise false.
		bool ProjectExists(std::string projDir);

		/// @brief Displays an informational message through the platform's message UI.
		void DisplayInfoMessage();
		/// @brief Shows an informational popup.
		/// @param msg Windows message containing input or window data.
		void PopUpInfo(const char* msg);

		/// @brief Displays an error message through the platform's message UI.
		void DisplayErrorMessage();
		/// @brief Shows an error popup and returns its user response when applicable.
		/// @param title Window title or dialog caption.
		/// @param msg Windows message containing input or window data.
		void PopUpError(const char* title, const char* msg);

		// Called according to FileMenuEvents.
		/// @brief Initializes the directories and configuration for a new editor project.
		/// @param path Filesystem path of the resource or project.
		void CreateNewProject(std::string& path);
		/// @brief Loads the selected editor project and its configured paths.
		/// @param path Filesystem path of the resource or project.
		void OpenProject(std::string& path);
		/// @brief Writes the current editor document to its configured path.
		/// @param path Filesystem path of the resource or project.
		void Save(std::string& path);
		/// @brief Selects a destination and writes the current editor document there.
		/// @param path Filesystem path of the resource or project.
		void SaveAs(std::string& path);
		/// @brief Opens the selected file or project for subsequent editing.
		/// @param path Filesystem path of the resource or project.
		void Open(std::string& path);

	private:
		/// @brief Persists editor preferences to the configuration file.
		void SaveEditorConfig();
		/// @brief Restores editor preferences from the configuration file.
		void LoadEditorConfig();
	};

	namespace ConfigKey
	{
		constexpr const char* GUI		 = "GUI Config";
		constexpr const char* FONT_SCALE = "Font Scale";
	} // namespace ConfigKey
} // namespace Editor
