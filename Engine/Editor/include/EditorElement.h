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

#include "Component/IComponent.h"

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
		public Common::IActor
	{
	public:
		virtual void AddChild(Common::IActor* actor) override;
		virtual void RemoveChild(Common::IActor* actor) override;
		virtual void RemoveComponent(Common::IComponent* component) override;
		virtual void RemoveAllComponents() override;

	public:
		// Deep copies transform from another IActor.
		virtual void CopyTransformFrom(IActor* actor) override;

		// Creates new IComponent with values from another IActor.
		virtual void CopyComponentsFrom(IActor* actor) override;

		// Deep copies all child Actors
		virtual void CopyChildObjectFrom(IActor* actor) override;

		// Shallow copies all child Actors.
		virtual void RefChildObjectFrom(IActor* actor) override;

	public:
		// Getters/Setters
		virtual Common::ActorData*							   GetData() override;
		virtual Common::ActorGroup							   GetActorGroup() const override;
		virtual Common::ActorGroup&							   GetActorGroupRef() override;
		virtual Common::ActorGroup*							   GetActorGroupPtr() override;
		virtual Common::FTDS::String						   GetName() override;
		virtual Common::FTDS::String&						   GetNameRef() override;
		virtual const int									   GetID() const override;
		virtual const bool&									   GetIsActive() const override;
		virtual bool&										   GetIsActiveRef() override;
		virtual Core::Transform*							   GetTransform() const override;
		virtual Editor::EditorElement*						   GetParent() const override;
		virtual Common::FTDS::DynamicArray<Common::IComponent*>* GetComponents() override;
		virtual Common::FTDS::DynamicArray<Common::IActor*>*	   GetChildActors() override;
		virtual const int&									   GetDrawOrder() const override;

		virtual void SetName(Common::FTDS::String&& name) override;
		virtual void SetIsActive(bool isActive) override;
		virtual void SetActorGroup(Common::ActorGroup group) override;
		virtual void SetState(Common::ActorState state) override;
		virtual void SetParent(Common::IActor* parent) override;
		virtual void SetTransform(Core::Transform* transform) override;
		virtual void SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components) override;
		virtual void SetChildActors(Common::FTDS::DynamicArray<IActor*>* children) override;
		virtual void SetDrawOrder(int order) override;

		virtual bool HasName(Common::FTDS::String&& name) override;
		virtual bool HasName(const char* name) override;
		virtual bool IsDead() override;

	public:
		virtual void SaveProperties(std::ofstream& ofs) override;
		virtual void SaveComponents(std::ofstream& ofs) override;

		virtual void LoadProperties(std::ifstream& ifs) override;
		virtual void LoadComponents(std::ifstream& ifs) override;

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

	private:
		Common::ActorData* mData;

		bool   mIsFocused;		// Is this item clicked on Foxtrot Editor's Hierarchy menu?
		bool   mIsDisplayed;	// Is this item displayed on Hierarchy menu during this frame?
		size_t mHierarchyLevel; // How many parent Actors are there for this object?

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
	extern "C" EDITOR_API EditorElement* CreateEditorElementFromActor(Common::IActor* actor, int id);
} // namespace Editor