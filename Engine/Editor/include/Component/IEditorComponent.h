#pragma once
#include "Core/InterfaceAliases.h"

namespace Editor
{
	template <typename COMP>
	class IEditorComponent
	{
	public:
		/// @brief Advances state needed only while operating in the editor.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void EditorUpdate(float deltaTime)									 = 0;
		/// @brief Submits editor-specific overlays and viewport graphics.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) = 0;
		/// @brief Updates the object's editor-facing controls.
		virtual void EditorUIUpdate()												 = 0;

	public:
		/// @brief Updates the comp used by subsequent operations.
		/// @param comp Replacement comp.
		void SetComp(COMP comp) { mComp = comp; }

	public:
		/// @brief Initializes the editor-facing component's actor context.
		/// @param comp Component instance associated with the actor or plugin.
		IEditorComponent(COMP* comp)
			: mComp(comp) {}

	protected:
		/// @brief Returns the comp used by this ieditor component.
		/// @return Borrowed access to the comp.
		COMP* GetComp() { return mComp; };

	private:
		COMP* mComp;
	};
} // namespace Editor
