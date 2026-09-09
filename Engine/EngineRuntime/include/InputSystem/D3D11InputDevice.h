#pragma once
#include "InputSystem/IInputDevice.h"

#include <Windows.h>
#include "Plugin/CoreExports.h"

namespace D3D11
{
	enum class GAMEPADBUTTON
	{
		INVALID,
		A,
		B,
		X,
		Y,

		LAST_FLAG
	};

	class D3D11Window;

	class D3D11InputDevice :
		public Core::IInputDevice
	{

	public:
		/// @brief Samples keyboard state and records press, hold, and release transitions.
		CORE_API virtual void DetectKeyboardInput();
		/// @brief Updates mouse button and client-coordinate state from a Windows message.
		/// @param msg Windows message containing input or window data.
		CORE_API void		 DetectMouseInput(MSG msg);
		/// @brief Computes mouse movement deltas for drag handling.
		/// @param deltaX Receives horizontal mouse movement.
		/// @param deltaY Receives vertical mouse movement.
		CORE_API virtual void DetectMouseDrag(int& deltaX, int& deltaY);

	public:
		/// @brief Tests whether the input remains pressed across frames.
		/// @param key Lookup key identifying the stored entry.
		/// @return True when the input remains pressed across frames; otherwise false.
		CORE_API bool KEY_HOLD(Core::KEYBOARD key) override;
		/// @brief Tests whether the input transitioned to pressed this frame.
		/// @param key Lookup key identifying the stored entry.
		/// @return True when the input transitioned to pressed this frame; otherwise false.
		CORE_API bool KEY_TAP(Core::KEYBOARD key) override;
		/// @brief Tests whether the input transitioned to released this frame.
		/// @param key Lookup key identifying the stored entry.
		/// @return True when the input transitioned to released this frame; otherwise false.
		CORE_API bool KEY_AWAY(Core::KEYBOARD key) override;
		/// @brief Tests whether the input is neither pressed nor transitioning.
		/// @param key Lookup key identifying the stored entry.
		/// @return True when the input is neither pressed nor transitioning; otherwise false.
		CORE_API bool KEY_NONE(Core::KEYBOARD key) override;

		/// @brief Tests whether the input remains pressed across frames.
		/// @param mouse Mouse button to query.
		/// @return True when the input remains pressed across frames; otherwise false.
		CORE_API bool MOUSE_HOLD(Core::MOUSE mouse) override;
		/// @brief Tests whether the input transitioned to pressed this frame.
		/// @param mouse Mouse button to query.
		/// @return True when the input transitioned to pressed this frame; otherwise false.
		CORE_API bool MOUSE_TAP(Core::MOUSE mouse) override;
		/// @brief Tests whether the input transitioned to released this frame.
		/// @param mouse Mouse button to query.
		/// @return True when the input transitioned to released this frame; otherwise false.
		CORE_API bool MOUSE_AWAY(Core::MOUSE mouse) override;
		/// @brief Tests whether the input is neither pressed nor transitioning.
		/// @param mouse Mouse button to query.
		/// @return True when the input is neither pressed nor transitioning; otherwise false.
		CORE_API bool MOUSE_NONE(Core::MOUSE mouse) override;

		/// @brief Reads the stored cursor X coordinate in client pixels.
		/// @return Signed cursor coordinate in client pixels.
		CORE_API int MOUSE_X() override;
		/// @brief Reads the stored cursor Y coordinate in client pixels.
		/// @return Signed cursor coordinate in client pixels.
		CORE_API int MOUSE_Y() override;

	public:
		/// @brief Confines the cursor to the scene viewport while it is being manipulated.
		/// @param window Window used by the operation.
		/// @param mousePos Cursor position used for viewport interaction.
		CORE_API void LockCursorInSceneViewport(Graphics::IWindow* window, Math::FTVector2 mousePos);
		/// @brief Releases the cursor confinement applied for viewport interaction.
		CORE_API void UnlockCursorOutOfSceneViewport();

		/// @brief Returns the mouse pos x used by this d3 d11 input device.
		/// @return Current mouse pos x.
		CORE_API virtual int GetMousePosX() const override;
		/// @brief Returns the mouse pos y used by this d3 d11 input device.
		/// @return Current mouse pos y.
		CORE_API virtual int GetMousePosY() const override;
		/// @brief Returns the mouse wheel delta used by this d3 d11 input device.
		/// @return Current mouse wheel delta.
		CORE_API virtual const float		   GetMouseWheelDelta() const override;

		/// @brief Updates the mouse position used by subsequent operations.
		/// @param pos Replacement mouse position.
		CORE_API void SetMousePosition(Math::FTVector2 pos);
		/// @brief Updates the mouse position used by subsequent operations.
		/// @param posX Horizontal position.
		/// @param posY Vertical position.
		CORE_API void SetMousePosition(int posX, int posY);
		/// @brief Updates the mouse wheel delta used by subsequent operations.
		/// @param delta Replacement mouse wheel delta.
		CORE_API void SetMouseWheelDelta(float delta);

	public:
		//////////////////////////////////////////
		////// Frame Update //////////////////////
		//////////////////////////////////////////

		/// @brief Advances frame-dependent state using the current time step.
		/// Updates input state. Called once per frame before input processing.
		/// @param window Window used by the operation.
		CORE_API virtual void Update(Core::IWindow* window) override;

		/// @brief Clears transient state before the next processing cycle.
		/// Resets per-frame input state. Called at end of frame.
		CORE_API virtual void Reset() override;

	public:
		/// @brief Initializes keyboard and mouse state used for frame transitions.
		CORE_API D3D11InputDevice();
		/// @brief Releases the resources managed by this instance during destruction.
		CORE_API ~D3D11InputDevice();

	private:
		// Mouse related data.
		int mMousePosX, mMousePosY;
		float		 mMouseWheelDelta;
		bool		 mIsDragging;

		int*										   mMouseCode;
		Common::FTDS::DynamicArray<Core::ButtonInput>* mMouseButtons;

	private:
		int*										   mKeyboardCode;
		Common::FTDS::DynamicArray<Core::ButtonInput>* mKeyboardButtons;
	};
} // namespace D3D11
