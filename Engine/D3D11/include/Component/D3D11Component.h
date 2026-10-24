#pragma once
#include "Core/InterfaceAliases.h"
#include "Component/IComponent.h"

#include "FTDS/Static/FTString.h"
#ifdef FOXTROT_EDITOR
	#include "CommandHistory.h"
#endif

namespace D3D11
{
	class Camera;
	class D3D11Renderer;
	class D3D11Component :
		public Core::IComponent
	{
	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		virtual void Initialize() override = 0;
		/// @brief Updates the up used by subsequent operations.
		virtual void Setup() override = 0;
		/// @brief Dispatches input for the current frame to the relevant engine objects.
		/// @param inputDevice Device exposing the current frame's input state.
		virtual void ProcessInput(Core::IInputDevice* inputDevice) = 0;
		/// @brief Advances frame-dependent state using the current time step.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void Update(float deltaTime) = 0;
		/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void LateUpdate(float deltaTime) = 0;
		/// @brief Submits this object's graphics work for the current frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void Render(Core::IRenderer* renderer, Core::ICamera* camInst) = 0;
		/// @brief Copies this object's state into the requested target or attaches its clone to the target actor.
		/// @param actor Actor participating in this operation.
		virtual void CloneTo(Core::IActor* actor) = 0;

	public:
		/// @brief Returns the name used by this d3 d11 component.
		/// @return Borrowed access to the name.
		virtual const char* GetName() = 0;
		/// @brief Returns the owner used by this d3 d11 component.
		/// @return Borrowed access to the owner.
		virtual Core::IActor* GetOwner() override;
		/// @brief Returns the update order used by this d3 d11 component.
		/// @return Current update order.

	public:
		/// @brief Initializes component lifecycle state and its owning actor.
		/// @param owner Actor or object associated with the new instance.
		/// @param updateOrder Order used when dispatching component updates.
		D3D11Component(Core::IActor* owner);
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~D3D11Component();

	private:
		Core::IActor* mOwner;

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Advances state needed only while operating in the editor.
		/// @param deltaTime Elapsed frame time in seconds.
		virtual void EditorUpdate(float deltaTime) = 0;
		/// @brief Submits editor-specific overlays and viewport graphics.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param camInst Camera supplying the view and projection for this draw.
		virtual void EditorRender(Core::IRenderer* renderer, Core::ICamera* camInst) = 0;
		/// @brief Updates the object's editor-facing controls.
		/// @param chInst Glyph instance whose metrics or vertices are used.
		virtual void EditorUIUpdate() = 0;
#endif // FOXTROT_EDITOR
	};
} // namespace D3D11
