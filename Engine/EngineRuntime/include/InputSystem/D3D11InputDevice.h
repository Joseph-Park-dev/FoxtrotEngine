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
		CORE_API virtual void DetectKeyboardInput();
		CORE_API void		 DetectMouseInput(MSG msg);
		CORE_API virtual void DetectMouseDrag(int& deltaX, int& deltaY);

	public:
		CORE_API bool KEY_HOLD(Core::KEYBOARD key) override;
		CORE_API bool KEY_TAP(Core::KEYBOARD key) override;
		CORE_API bool KEY_AWAY(Core::KEYBOARD key) override;
		CORE_API bool KEY_NONE(Core::KEYBOARD key) override;

		CORE_API bool MOUSE_HOLD(Core::MOUSE mouse) override;
		CORE_API bool MOUSE_TAP(Core::MOUSE mouse) override;
		CORE_API bool MOUSE_AWAY(Core::MOUSE mouse) override;
		CORE_API bool MOUSE_NONE(Core::MOUSE mouse) override;

		CORE_API int MOUSE_X() override;
		CORE_API int MOUSE_Y() override;

	public:
		CORE_API void LockCursorInSceneViewport(Graphics::IWindow* window, Math::FTVector2 mousePos);
		CORE_API void UnlockCursorOutOfSceneViewport();

		CORE_API virtual int GetMousePosX() const override;
		CORE_API virtual int GetMousePosY() const override;
		CORE_API virtual const float		   GetMouseWheelDelta() const override;

		CORE_API void SetMousePosition(Math::FTVector2 pos);
		CORE_API void SetMousePosition(int posX, int posY);
		CORE_API void SetMouseWheelDelta(float delta);

	public:
		//////////////////////////////////////////
		////// Frame Update //////////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Updates input state. Called once per frame before input processing.
		/// </summary>
		CORE_API virtual void Update(Core::IWindow* window) override;

		/// <summary>
		/// Resets per-frame input state. Called at end of frame.
		/// </summary>
		CORE_API virtual void Reset() override;

	public:
		CORE_API D3D11InputDevice();
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