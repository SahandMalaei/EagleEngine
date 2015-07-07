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
};

#endif