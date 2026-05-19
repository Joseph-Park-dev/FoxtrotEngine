#pragma once
#include "InputSystem/IInputDevice.h"

#include <Windows.h>

namespace D3D11
{
	enum class KEYBOARD
	{
		A,
		D,
		W,
		S,
		SHIFT,
		SPACE,
		LAST_FLAG
	};

	enum class MOUSE
	{
		MOUSE_LEFT,
		MOUSE_RIGHT,
		MOUSE_MIDDLE,

		LAST_FLAG
	};

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
		virtual void DetectKeyboardInput();
		void		 DetectMouseInput(MSG msg);
		virtual void DetectMouseDrag(int& deltaX, int& deltaY);

	public:
		bool KEY_HOLD(KEYBOARD key);
		bool KEY_TAP(KEYBOARD key);
		bool KEY_AWAY(KEYBOARD key);
		bool KEY_NONE(KEYBOARD key);

		bool MOUSE_HOLD(MOUSE mouse);
		bool MOUSE_TAP(MOUSE mouse);
		bool MOUSE_AWAY(MOUSE mouse);
		bool MOUSE_NONE(MOUSE mouse);

		unsigned int MOUSE_X();
		unsigned int MOUSE_Y();

	public:
		void LockCursorInSceneViewport(D3D11Window* window, Math::FTVector2 mousePos);
		void UnlockCursorOutOfSceneViewport();

		virtual const unsigned int GetMousePosX() const override;
		virtual const unsigned int GetMousePosY() const override;
		virtual const float		   GetMouseWheelDelta() const override;

		void SetMousePosition(Math::FTVector2 pos);
		void SetMousePosition(unsigned int posX, unsigned int posY);
		void SetMouseWheelDelta(float delta);

	public:
		//////////////////////////////////////////
		////// Frame Update //////////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Updates input state. Called once per frame before input processing.
		/// </summary>
		virtual void Update(Core::IWindow* window) override;

		/// <summary>
		/// Resets per-frame input state. Called at end of frame.
		/// </summary>
		virtual void Reset() override;

	public:
		D3D11InputDevice();
		~D3D11InputDevice();

	private:
		// Mouse related data.
		unsigned int mMousePosX, mMousePosY;
		float		 mMouseWheelDelta;
		bool		 mIsDragging;

		int*										   mMouseCode;
		Common::FTDS::DynamicArray<Core::ButtonInput>* mMouseButtons;

	private:
		int*										   mKeyboardCode;
		Common::FTDS::DynamicArray<Core::ButtonInput>* mKeyboardButtons;
	};
} // namespace D3D11