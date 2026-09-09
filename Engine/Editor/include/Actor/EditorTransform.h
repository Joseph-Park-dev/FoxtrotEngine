#pragma once
#include "Actor/Transform.h"

namespace Editor
{
	class EditorElement;
	class EditorTransform : public Core::Transform
	{
	public:
		/// @brief Builds the editor controls for inspecting and modifying this object's state.
		void UpdateUI();

	public:
		/// @brief Exposes the position relative to the actor's parent.
		/// @return Borrowed access to the local position.
		/// @note Changes through the returned reference affect this object's stored state.
		Math::FTVector3& LocalPosition() const;
		/// @brief Exposes the scale relative to the actor's parent.
		/// @return Borrowed access to the local scale.
		/// @note Changes through the returned reference affect this object's stored state.
		Math::FTVector3& LocalScale() const;
		/// @brief Exposes the rotation relative to the actor's parent.
		/// @return Borrowed access to the local rotation.
		/// @note Changes through the returned reference affect this object's stored state.
		Math::FTVector3& LocalRotation() const;

		// World Transformation
		/// @brief Exposes the actor's position in world coordinates.
		/// @return Borrowed access to the world position.
		/// @note Changes through the returned reference affect this object's stored state.
		Math::FTVector3& WorldPosition() const;
		/// @brief Exposes the actor's scale in world coordinates.
		/// @return Borrowed access to the world scale.
		/// @note Changes through the returned reference affect this object's stored state.
		Math::FTVector3& WorldScale() const;
		/// @brief Exposes the actor's rotation in world coordinates.
		/// @return Borrowed access to the world rotation.
		/// @note Changes through the returned reference affect this object's stored state.
		Math::FTVector3& WorldRotation() const;

	public:
		/// @brief Initializes an editor-controlled transform for its actor.
		/// @param element Element to insert or edit.
		EditorTransform(Editor::EditorElement* element);
	};
} // namespace Editor
