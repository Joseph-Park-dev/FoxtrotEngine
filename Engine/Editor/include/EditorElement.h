// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A holder object that has the values of an Actor.
/// This will be on Foxtrot Editor, and the user can modify its values.
/// Values of EditorElements will be saved as an ActorData in .Chunk file,
/// and will be used to create Actors in the produced game.
/// </summary>

#pragma once
#include "Actor/IActor.h"

namespace Core
{
	class Scene;
	class Actor;
	class FTPremade;
} // namespace Core

namespace D3D11
{
	class D3D11Renderer;
}

namespace Editor
{
#define ACTORNAME_MAX 100
	class EditorLayer;
	class EditorScene;

	class EditorElement :
		public Core::IActor
	{
	public:
		const bool	 GetIsFocused() const;
		const size_t GetHierarchyLevel() const;
		const bool	 GetIsDisplayed() const;

		virtual void SetIsFocused(bool isFocused);
		void		 SetHierarchyLevel(size_t lv);
		void		 SetIsDisplayed(bool isDisplayed);

	public:
		void Initialize();
		//// Updates editor specific features -> this will be omitted from the produced game.
		// void EditorUpdate(float deltaTime);

		//// Renders editor specific features -> this will be omitted from the produced game.
		// void EditorRender(D3D11::D3D11Renderer* renderer);

	public:
		/// <summary>
		/// Updates UIs displayed on Inspector menu.
		/// </summary>
		/// <param name="isPremade : ">Is this object premade?</param>
		virtual void UpdateUI(bool isPremade);

	public:
		/// <summary>
		/// Constructor that creates empty EditorElement.
		/// It should be added to the EditorScene manually.
		EditorElement(int id);

		/// <summary>
		/// Constructor that deep-copies an Actor object.
		/// </summary>
		/// <param name="origin : ">An Actor object to deep-copy values from.</param>
		EditorElement(IActor* actor, int id);

		/// <summary>
		/// Copy constructors controlling if the object is deep copied.
		/// </summary>
		/// <param name="actor">Actor being copied.</param>
		/// <param name="deepCpy">Perform deep copy through child Actors?</param>
		EditorElement(IActor* actor, int id, bool deepCpyChild);

		/// <summary>
		/// Constructor that is used for FTPremade origin.
		/// This Fetches the FTPremade origin, makes EditorElement.
		/// Not recommended to use outside of FTPremade
		EditorElement(Core::FTPremade* premade, int id);

	public:
		// Deep copies all child Actors
		void CopyChildObjectFrom(IActor* actor) override;

	private:
		bool   mIsFocused;		// Is this item clicked on Foxtrot Editor's Hierarchy menu?
		size_t mHierarchyLevel; // How many parent Actors are there for this object?

		bool mIsDisplayed; // Is this item displayed on Hierarchy menu during this frame?

	private:
		void UpdateActorName();
		void UpdateActorGroup();
		void UpdateDrawOrder();

		void UpdateTransformUI();

		void UpdateComponentsUI();
		void DisplayCompSelectionPopup();

		void UpdateMakePremade();
		void SwitchTransformToEditor();
	};

#include "Plugin/EditorExports.h"

	constexpr const char* CREATE_EDITOR_ELEMENT_FROM_ACTOR = "CreateEditorElementFromActor";
	// extern "C" EDITOR_API EditorElement* CreateEditorElement(int id);
	extern "C" EDITOR_API EditorElement* CreateEditorElementFromActor(Core::Actor* actor, int id);
} // namespace Editor