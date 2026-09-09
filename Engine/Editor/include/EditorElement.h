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
		/// @brief Registers an actor in this actor's child collection.
		/// @param actor Actor participating in this operation.
		virtual void AddChild(Common::IActor* actor) override;
		/// @brief Detaches an actor from this actor's child collection.
		/// @param actor Actor participating in this operation.
		virtual void RemoveChild(Common::IActor* actor) override;
		/// @brief Removes the specified component from the actor's component collection.
		/// @param component Component instance associated with the actor or plugin.
		virtual void RemoveComponent(Common::IComponent* component) override;
		/// @brief Removes all components associated with the actor.
		virtual void RemoveAllComponents() override;

	public:
		// Deep copies transform from another IActor.
		/// @brief Copies spatial state from the source actor into this actor's transform.
		/// @param actor Actor participating in this operation.
		virtual void CopyTransformFrom(IActor* actor) override;

		// Creates new IComponent with values from another IActor.
		/// @brief Clones the source actor's components for this actor.
		/// @param actor Actor participating in this operation.
		virtual void CopyComponentsFrom(IActor* actor) override;

		// Deep copies all child Actors
		/// @brief Clones the source actor's child hierarchy.
		/// @param actor Actor participating in this operation.
		virtual void CopyChildObjectFrom(IActor* actor) override;

		// Shallow copies all child Actors.
		/// @brief Associates the source actor's children with this editor or runtime actor.
		/// @param actor Actor participating in this operation.
		virtual void RefChildObjectFrom(IActor* actor) override;

	public:
		// Getters/Setters
		/// @brief Returns the data used by this editor element.
		/// @return Borrowed access to the data.
		virtual Common::ActorData*							   GetData() override;
		/// @brief Returns the actor group used by this editor element.
		/// @return Current actor group.
		virtual Common::ActorGroup							   GetActorGroup() const override;
		/// @brief Returns the actor group ref used by this editor element.
		/// @return Borrowed access to the actor group ref.
		/// @note Changes through the returned reference affect this object's stored state.
		virtual Common::ActorGroup&							   GetActorGroupRef() override;
		/// @brief Returns the actor group ptr used by this editor element.
		/// @return Borrowed access to the actor group ptr.
		virtual Common::ActorGroup*							   GetActorGroupPtr() override;
		/// @brief Returns the name used by this editor element.
		/// @return Current name.
		virtual Common::FTDS::String						   GetName() override;
		/// @brief Returns the name ref used by this editor element.
		/// @return Borrowed access to the name ref.
		/// @note Changes through the returned reference affect this object's stored state.
		virtual Common::FTDS::String&						   GetNameRef() override;
		/// @brief Returns the id used by this editor element.
		/// @return Current id.
		virtual const int									   GetID() const override;
		/// @brief Returns the is active used by this editor element.
		/// @return Current value of the is active flag.
		virtual const bool&									   GetIsActive() const override;
		/// @brief Returns the is active ref used by this editor element.
		/// @return Current value of the is active ref flag.
		virtual bool&										   GetIsActiveRef() override;
		/// @brief Returns the transform used by this editor element.
		/// @return Borrowed access to the transform.
		virtual Core::Transform*							   GetTransform() const override;
		/// @brief Returns the parent used by this editor element.
		/// @return Borrowed access to the parent.
		virtual Editor::EditorElement*						   GetParent() const override;
		/// @brief Returns the components used by this editor element.
		/// @return Borrowed access to the components.
		virtual Common::FTDS::DynamicArray<Common::IComponent*>* GetComponents() override;
		/// @brief Returns the child actors used by this editor element.
		/// @return Borrowed access to the child actors.
		virtual Common::FTDS::DynamicArray<Common::IActor*>*	   GetChildActors() override;
		/// @brief Returns the draw order used by this editor element.
		/// @return Borrowed access to the draw order.
		virtual const int&									   GetDrawOrder() const override;

		/// @brief Updates the name used by subsequent operations.
		/// @param name Replacement name.
		virtual void SetName(Common::FTDS::String&& name) override;
		/// @brief Updates the is active used by subsequent operations.
		/// @param isActive Replacement is active.
		virtual void SetIsActive(bool isActive) override;
		/// @brief Updates the actor group used by subsequent operations.
		/// @param group Replacement actor group.
		virtual void SetActorGroup(Common::ActorGroup group) override;
		/// @brief Updates the state used by subsequent operations.
		/// @param state Replacement state.
		virtual void SetState(Common::ActorState state) override;
		/// @brief Updates the parent used by subsequent operations.
		/// @param parent Replacement parent.
		virtual void SetParent(Common::IActor* parent) override;
		/// @brief Updates the transform used by subsequent operations.
		/// @param transform Replacement transform.
		virtual void SetTransform(Core::Transform* transform) override;
		/// @brief Updates the components used by subsequent operations.
		/// @param components Replacement components.
		virtual void SetComponents(Common::FTDS::DynamicArray<Common::IComponent*>* components) override;
		/// @brief Updates the child actors used by subsequent operations.
		/// @param children Replacement child actors.
		virtual void SetChildActors(Common::FTDS::DynamicArray<IActor*>* children) override;
		/// @brief Updates the draw order used by subsequent operations.
		/// @param order Replacement draw order.
		virtual void SetDrawOrder(int order) override;

		/// @brief Compares the actor's stored name with the supplied name.
		/// @param name Name used to identify the requested object or interface.
		/// @return True when the compared values match; otherwise false.
		virtual bool HasName(Common::FTDS::String&& name) override;
		/// @brief Compares the actor's stored name with the supplied name.
		/// @param name Name used to identify the requested object or interface.
		/// @return True when the compared values match; otherwise false.
		virtual bool HasName(const char* name) override;
		/// @brief Tests whether the actor state requests deferred destruction.
		/// @return True when the actor state requests deferred destruction; otherwise false.
		virtual bool IsDead() override;

	public:
		/// @brief Serializes this object's persistent properties to a .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		/// @note Writes to the supplied stream at its current position.
		virtual void SaveProperties(std::ofstream& ofs) override;
		/// @brief Serializes the actor's attached components to the .chunk stream.
		/// @param ofs Output stream receiving the serialized data.
		virtual void SaveComponents(std::ofstream& ofs) override;

		/// @brief Restores this object's persistent properties from a .chunk stream.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @note Advances the stream position and updates the destination state.
		virtual void LoadProperties(std::ifstream& ifs) override;
		/// @brief Restores actor components through the available plugin factories.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		virtual void LoadComponents(std::ifstream& ifs) override;

	public:
		/// @brief Returns the is focused used by this editor element.
		/// @return Current value of the is focused flag.
		const bool	 GetIsFocused() const;
		/// @brief Returns the hierarchy level used by this editor element.
		/// @return Current hierarchy level.
		const size_t GetHierarchyLevel() const;
		/// @brief Returns the is displayed used by this editor element.
		/// @return Current value of the is displayed flag.
		const bool	 GetIsDisplayed() const;

		/// @brief Updates the is focused used by subsequent operations.
		/// @param isFocused Replacement is focused.
		virtual void SetIsFocused(bool isFocused);
		/// @brief Updates the hierarchy level used by subsequent operations.
		/// @param lv Replacement hierarchy level.
		void		 SetHierarchyLevel(size_t lv);
		/// @brief Updates the is displayed used by subsequent operations.
		/// @param isDisplayed Replacement is displayed.
		void		 SetIsDisplayed(bool isDisplayed);

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		void Initialize();
		//// Updates editor specific features -> this will be omitted from the produced game.
		// void EditorUpdate(float deltaTime);

		//// Renders editor specific features -> this will be omitted from the produced game.
		// void EditorRender(D3D11::D3D11Renderer* renderer);

	public:
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
		/// Updates UIs displayed on Inspector menu.
		/// <param name="isPremade : ">Is this object premade?</param>
		/// @param isPremade Whether the actor represents a premade resource.
		virtual void UpdateUI(bool isPremade);

	public:
		/// @brief Initializes an actor wrapper with editor selection and hierarchy state.
		/// Constructor that creates empty EditorElement.
		/// It should be added to the EditorScene manually.
		/// @param id Actor or entry identifier.
		EditorElement(int id);

		/// @brief Initializes an actor wrapper with editor selection and hierarchy state.
		/// Constructor that deep-copies an Actor object.
		/// <param name="origin : ">An Actor object to deep-copy values from.</param>
		/// @param actor Actor participating in this operation.
		/// @param id Actor or entry identifier.
		EditorElement(IActor* actor, int id);

		/// @brief Initializes an actor wrapper with editor selection and hierarchy state.
		/// Copy constructors controlling if the object is deep copied.
		/// <param name="actor">Actor being copied.</param>
		/// <param name="deepCpy">Perform deep copy through child Actors?</param>
		/// @param actor Actor participating in this operation.
		/// @param id Actor or entry identifier.
		/// @param deepCpyChild Whether child actors are cloned instead of sharing references.
		EditorElement(IActor* actor, int id, bool deepCpyChild);

		/// @brief Initializes an actor wrapper with editor selection and hierarchy state.
		/// Constructor that is used for FTPremade origin.
		/// This Fetches the FTPremade origin, makes EditorElement.
		/// Not recommended to use outside of FTPremade
		/// @param premade Premade resource used as an actor template.
		/// @param id Actor or entry identifier.
		EditorElement(Core::FTPremade* premade, int id);

	private:
		Common::ActorData* mData;

		bool   mIsFocused;		// Is this item clicked on Foxtrot Editor's Hierarchy menu?
		bool   mIsDisplayed;	// Is this item displayed on Hierarchy menu during this frame?
		size_t mHierarchyLevel; // How many parent Actors are there for this object?

	private:
		/// @brief Applies an edited actor name through the editor controls.
		void UpdateActorName();
		/// @brief Applies an edited actor group through the editor controls.
		void UpdateActorGroup();
		/// @brief Applies the draw-order value entered in the editor.
		void UpdateDrawOrder();

		/// @brief Builds controls for editing the actor's transform.
		void UpdateTransformUI();

		/// @brief Builds property controls for the selected actor's components.
		void UpdateComponentsUI();
		/// @brief Draws the popup used to choose a component type.
		void DisplayCompSelectionPopup();

		/// @brief Handles the editor action that saves the current actor as a premade resource.
		void UpdateMakePremade();
		/// @brief Replaces runtime transform behavior with the editor transform representation.
		void SwitchTransformToEditor();
	};

#include "Plugin/EditorExports.h"

	constexpr const char* CREATE_EDITOR_ELEMENT_FROM_ACTOR = "CreateEditorElementFromActor";
	// extern "C" EDITOR_API EditorElement* CreateEditorElement(int id);
	/// @brief Builds an editor-facing actor wrapper for inspecting and manipulating the source actor.
	/// @param actor Actor participating in this operation.
	/// @param id Actor or entry identifier.
	/// @return Created editor element from actor instance or resource.
	extern "C" EDITOR_API EditorElement* CreateEditorElementFromActor(Common::IActor* actor, int id);
} // namespace Editor
