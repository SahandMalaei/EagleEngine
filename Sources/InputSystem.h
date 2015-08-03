#ifndef _INPUT_H
#define _INPUT_H 1

#include "Eagle.h"

#ifndef PLATFORM_WP8
	#include <Include/DirectX/dinput.h>
	#include <Include/DirectX/dinputd.h>
#endif

#define SAFE_DELETE(p){if(p){delete (p); (p) = NULL;}}
#define SAFE_RELEASE(p){if(p){(p)->Release(); (p) = NULL;}}

#ifdef PLATFORM_WP8
	#define INPUT_SUPPORTED_TOUCH_COUNT 10
	#define INPUT_TOUCH_ANY -1
#endif

struct Point
{
	float x, y;
};

#define INPUT_MAX_KEY_SEQUENCE_LENGTH 256

namespace ProjectEagle
{
	enum KeyboardKey;
	enum MouseButton;

	class InputSystem
	{
#ifdef PLATFORM_WP8
		friend ref class ApplicationWP8;
#endif
		friend class EagleEngine;

	private:
#ifndef PLATFORM_WP8
		HWND m_windowHandle;

		IDirectInput8 *m_directInput;
		IDirectInputDevice8 *m_keyboard;
		IDirectInputDevice8 *m_joystick;
		IDirectInputDevice8 *m_mouse;

		DIMOUSESTATE m_mouseState;
		DIJOYSTATE2 m_joystickState;
#endif

		bool m_joystickValidity;

		char m_keyState[256];
		bool m_oldKeys[256];
		float m_keyHoldDuration[256];
		float m_keyHoldStart[256];
		short m_keyTapCount[256];
		short m_keyTapFrequency[256];
		int m_keyTotalPressCount[256];

#ifdef PLATFORM_WP8
		bool m_touchList[INPUT_SUPPORTED_TOUCH_COUNT];
		bool m_oldTouchList[INPUT_SUPPORTED_TOUCH_COUNT];

		int m_touchCount;

		bool m_backButtonPressed;

		bool m_suppressBackButtonHandler;
#endif

		bool m_oldJoystickButtons[128];

		short m_keySequence[INPUT_MAX_KEY_SEQUENCE_LENGTH];

#ifndef PLATFORM_WP8
		Vector2 m_mousePosition;
		Vector2 m_mouseMove;
#else
		Vector2 m_touchPositionList[INPUT_SUPPORTED_TOUCH_COUNT];
		int m_touchIDList[INPUT_SUPPORTED_TOUCH_COUNT];
		Vector2 m_touchMoveList[INPUT_SUPPORTED_TOUCH_COUNT];
#endif


#ifndef PLATFORM_WP8
		bool m_oldButtons[4];
		float m_buttonHoldDuration[256];
		float m_buttonHoldStart[256];
		short m_buttonTapCount[4];
		short m_buttonTapFrequency[4];
		int m_buttonTotalPressCount[4];
#else
		float m_touchHoldDuration[INPUT_SUPPORTED_TOUCH_COUNT];
		float m_touchHoldStart[INPUT_SUPPORTED_TOUCH_COUNT];
		int m_touchTapCount[INPUT_SUPPORTED_TOUCH_COUNT];
		int m_touchTapFrequency[INPUT_SUPPORTED_TOUCH_COUNT];
		int m_touchTotalPressCount[INPUT_SUPPORTED_TOUCH_COUNT];
#endif

		int m_wheelMove;

		Vector2 m_joystickAxis;

		bool m_inputExclusive;

		Timer m_tapTimer;

		void addToKeySequence(int k);

		void initialize();
		void update();

		void resetKeyStates();

#ifdef PLATFORM_WP8
		bool m_gyrometerPresent;
		bool m_accelerometerPresent;
		bool m_inclinometerPresent;

		Vector3 m_gyrometerData;
		Vector3 m_accelerometerData;
		Vector3 m_inclinometerData;
#endif

	public:
		InputSystem();
		virtual ~InputSystem();

		bool getKeyState(KeyboardKey key);
		bool getJoystickButtonState(int b);

#ifndef PLATFORM_WP8
		Vector2 getMousePosition();

		bool getMouseButtonState(MouseButton button);

		bool isMouseButtonDown(MouseButton button);
		bool isMouseButtonUp(MouseButton button);
		bool isMouseButtonReleased(MouseButton button);
		bool isMouseButtonPressed(MouseButton button);

		float getMouseButtonHoldDuration(MouseButton button);
		void resetMouseButtonHoldDuration(MouseButton button);

		int getMouseButtonTapFrequency(MouseButton button);
		void resetMouseButtonTapFrequency(MouseButton button);

		int getMouseButtonTotalPressCount(MouseButton button);

		Vector2 getMouseMove();
#else
		Vector2 getMousePosition(int touchID = INPUT_TOUCH_ANY);

		bool getMouseButtonState(int touchID = INPUT_TOUCH_ANY);

		bool isMouseButtonDown(int touchID = INPUT_TOUCH_ANY);
		bool isMouseButtonUp(int touchID = INPUT_TOUCH_ANY);
		bool isMouseButtonReleased(int touchID = INPUT_TOUCH_ANY);
		bool isMouseButtonPressed(int touchID = INPUT_TOUCH_ANY);

		int getMouseButtonHoldDuration(int touchID = INPUT_TOUCH_ANY);
		void resetMouseButtonHoldDuration(int touchID = 0);

		int getMouseButtonTapFrequency(int touchID = 0);
		void resetMouseButtonTapFrequency(int touchID = 0);

		int getMouseButtonTotalPressCount(int touchID = 0);

		Vector2 getMouseMove(int touchID = INPUT_TOUCH_ANY);

		bool isBackButtonPressed();

		void suppressBackButtonHandling();
		void dontSuppressBackButtonHandling();
#endif

		bool isKeyDown(KeyboardKey key);
		bool isKeyUp(KeyboardKey key);
		bool isKeyReleased(KeyboardKey key);
		bool isKeyPressed(KeyboardKey key);

		float getKeyHoldDuration(KeyboardKey key);
		void resetKeyHoldDuration(KeyboardKey key);

		int getKeyTapFrequency(KeyboardKey key);

		int getKeyTotalPressCount(KeyboardKey key);

		bool checkForKeySequence(std::string sequence);
		void clearKeySequence();

		long getMouseWheelMove();

		Vector2 getJoystickAxis();

		bool isJoystickValid();
		bool isJoystickButtonDown(int b);
		bool isJoystickButtonUp(int b);
		bool isJoystickButtonReleased(int b);
		bool isJoystickButtonPressed(int b);

		void setInputExclusive();
		void setInputNonExclusive();

		void resetOldKeys();

		void waitForKeyPress();

#ifdef PLATFORM_WP8
		bool isGyrometerPresent();
		bool isAccelerometerPresent();
		bool isInclinometerPresent();

		Vector3 getGyrometerData();
		Vector3 getAccelerometerData();
		Vector3 getInclinometerData();
#endif
	};

#ifndef PLATFORM_WP8
	struct DI_ENUM_CONTEXT
	{
		DIJOYCONFIG* pPreferredJoyConfig;
		bool bPreferredJoyConfigValid;
	};
#endif

	enum KeyboardKey
	{
		Keyboard_Escape = 0x01,
		Keyboard_num1 = 0x02,
		Keyboard_num2 = 0x03,
		Keyboard_num3 = 0x04,
		Keyboard_num4 = 0x05,
		Keyboard_num5 = 0x06,
		Keyboard_num6 = 0x07,
		Keyboard_num7 = 0x08,
		Keyboard_num8 = 0x09,
		Keyboard_num9 = 0x0A,
		Keyboard_num0 = 0x0B,
		Keyboard_Minus = 0x0C,
		Keyboard_Equals = 0x0D,
		Keyboard_Back = 0x0E,
		Keyboard_Tab = 0x0F,
		Keyboard_Q = 0x10,
		Keyboard_W = 0x11,
		Keyboard_E = 0x12,
		Keyboard_R = 0x13,
		Keyboard_T = 0x14,
		Keyboard_Y = 0x15,
		Keyboard_U = 0x16,
		Keyboard_I = 0x17,
		Keyboard_O = 0x18,
		Keyboard_P = 0x19,
		Keyboard_LBracket = 0x1A,
		Keyboard_RBracket = 0x1B,
		Keyboard_Return = 0x1C,
		Keyboard_LControl = 0x1D,
		Keyboard_A = 0x1E,
		Keyboard_S = 0x1F,
		Keyboard_D = 0x20,
		Keyboard_F = 0x21,
		Keyboard_G = 0x22,
		Keyboard_H = 0x23,
		Keyboard_J = 0x24,
		Keyboard_K = 0x25,
		Keyboard_L = 0x26,
		Keyboard_Semicolon = 0x27,
		Keyboard_Apostrophe = 0x28,
		Keyboard_Grave = 0x29,
		Keyboard_LShift = 0x2A,
		Keyboard_Backslash = 0x2B,
		Keyboard_Z = 0x2C,
		Keyboard_X = 0x2D,
		Keyboard_C = 0x2E,
		Keyboard_V = 0x2F,
		Keyboard_B = 0x30,
		Keyboard_N = 0x31,
		Keyboard_M = 0x32,
		Keyboard_Comma = 0x33,
		Keyboard_Period = 0x34,
		Keyboard_Slash = 0x35,
		Keyboard_RShift = 0x36,
		Keyboard_Multiply = 0x37,
		Keyboard_LAlt = 0x38,
		Keyboard_Space = 0x39,
		Keyboard_Capital = 0x3A,
		Keyboard_F1 = 0x3B,
		Keyboard_F2 = 0x3C,
		Keyboard_F3 = 0x3D,
		Keyboard_F4 = 0x3E,
		Keyboard_F5 = 0x3F,
		Keyboard_F6 = 0x40,
		Keyboard_F7 = 0x41,
		Keyboard_F8 = 0x42,
		Keyboard_F9 = 0x43,
		Keyboard_F10 = 0x44,
		Keyboard_NumLock = 0x45,
		Keyboard_ScrollLock = 0x46,
		Keyboard_Numpad7 = 0x47,
		Keyboard_Numpad8 = 0x48,
		Keyboard_Numpad9 = 0x49,
		Keyboard_Subtract = 0x4A,
		Keyboard_Numpad4 = 0x4B,
		Keyboard_Numpad5 = 0x4C,
		Keyboard_Numpad6 = 0x4D,
		Keyboard_Plus = 0x4E,
		Keyboard_Numpad1 = 0x4F,
		Keyboard_Numpad2 = 0x50,
		Keyboard_Numpad3 = 0x51,
		Keyboard_Numpad0 = 0x52,
		Keyboard_Dot = 0x53,
		Keyboard_F11 = 0x57,
		Keyboard_F12 = 0x58,
		Keyboard_Up = 0xC8,  
		Keyboard_PgUp = 0xC9,
		Keyboard_Left = 0xCB, 
		Keyboard_Right = 0xCD,
		Keyboard_End = 0xCF,
		Keyboard_Down = 0xD0,
		Keyboard_PgDown = 0xD1,
		Keyboard_Insert = 0xD2,
		Keyboard_Delete = 0xD3
	};

	enum MouseButton
	{
		Mouse_Left = 0,
		Mouse_Right = 1,
		Mouse_Middle = 2
	};
};

#endif