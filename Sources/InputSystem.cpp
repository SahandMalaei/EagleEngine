#include "Eagle.h"

namespace ProjectEagle
{
	InputSystem::InputSystem()
	{
		m_inputExclusive = 0;
	}

	void InputSystem::initialize()
	{
#ifndef PLATFORM_WP8
		m_windowHandle = graphics.getWidnowHandle();

		DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
		
		//
		// keyboard
		//

		m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
		m_keyboard->SetDataFormat(&c_dfDIKeyboard);

		if(!m_inputExclusive)
		{
			m_keyboard->SetCooperativeLevel(m_windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		}
		else
		{
			m_keyboard->SetCooperativeLevel(m_windowHandle, DISCL_EXCLUSIVE);
		}

		m_keyboard->Acquire();

		//
		// mouse
		//

		m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
		m_mouse->SetDataFormat(&c_dfDIMouse);

		if(!m_inputExclusive)
		{
			m_mouse->SetCooperativeLevel(m_windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		}
		else
		{
			m_mouse->SetCooperativeLevel(m_windowHandle, DISCL_EXCLUSIVE);
		}

		m_mouse->Acquire();

		for(int i = 0; i < 256; ++i)
		{
			m_oldKeys[i] = 0;
			m_keyState[i] = 0;

			m_keyHoldDuration[i] = 0;
			m_keyHoldStart[i] = 0;

			m_keyTapCount[i] = 0;
			m_keyTapFrequency[i] = 0;

			m_keyTotalPressCount[i] = 0;

			m_keySequence[i] = -1;
		}

		for(int i = 0; i < 4; ++i)
		{
			m_oldButtons[i] = 0;

			m_buttonHoldDuration[i] = 0;
			m_buttonHoldStart[i] = 0;

			m_buttonTapCount[i] = 0;
			m_buttonTapFrequency[i] = 0;

			m_buttonTotalPressCount[i] = 0;
		}

		m_tapTimer.reset();

		//
		// joystick
		//

		m_joystickValidity = 0;

		DIJOYCONFIG conf = {0};
		DI_ENUM_CONTEXT enumContext;
		enumContext.pPreferredJoyConfig = &conf;
		enumContext.bPreferredJoyConfigValid = 0;

		IDirectInputJoyConfig8 *joyConfig = 0;
		if(FAILED(m_directInput->QueryInterface(IID_IDirectInputJoyConfig8, (void **)&joyConfig)))
		{
			m_joystickValidity = 0;

			return;
		}

		conf.dwSize = sizeof(conf);
		if(SUCCEEDED(joyConfig->GetConfig(0, &conf, DIJC_GUIDINSTANCE))) 
		{
			enumContext.bPreferredJoyConfigValid = 1;
		}

		SAFE_RELEASE(joyConfig);

		if(FAILED(m_directInput->CreateDevice(GUID_Joystick, &m_joystick, 0)))
		{
			m_joystickValidity = 0;
			
			return;
		}

		if(FAILED(m_joystick->SetDataFormat(&c_dfDIJoystick2)))
		{
			m_joystickValidity = 0;

			return;
		}

		if(!m_inputExclusive)
		{
			m_joystick->SetCooperativeLevel(m_windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		}
		else 
		{
			m_joystick->SetCooperativeLevel(m_windowHandle, DISCL_EXCLUSIVE);
		}

		m_joystick->Acquire();

		for(int i = 0; i < 128; ++i)
		{
			m_oldJoystickButtons[i] = 0;
		}

		m_joystickValidity = 1;

#else
		for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
		{
			m_touchList[i] = 0;
			m_oldTouchList[i] = 0;

			m_touchHoldDuration[i] = 0;
			m_touchHoldStart[i] = 0;

			m_touchTapCount[i] = 0;
			m_touchTapFrequency[i] = 0;

			m_touchTotalPressCount[i] = 0;
		}

		m_tapTimer.reset();

		m_backButtonPressed = 0;

#endif
	}

	InputSystem::~InputSystem()
	{
		/*di->Release();
		keyboard->Release();
		mouse->Release();
		joystick->Release();*/
	}

	void InputSystem::update()
	{
#ifndef PLATFORM_WP8
		m_keyboard->Poll();

		if(!SUCCEEDED(m_keyboard->GetDeviceState(256, (LPVOID)&m_keyState)))
		{
			m_keyboard->Acquire();
		}

		m_mouse->Poll();

		if(!SUCCEEDED(m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_mouseState)))
		{
			m_mouse->Acquire();
		}

		m_mouseMove.x = m_mouseState.lX;
		m_mouseMove.y = m_mouseState.lY;

		m_wheelMove = m_mouseState.lZ;

		POINT mousePoint;
		GetCursorPos(&mousePoint);
		ScreenToClient(m_windowHandle, &mousePoint);
		m_mousePosition.x = mousePoint.x;
		m_mousePosition.y = mousePoint.y;

		if(m_joystickValidity)
		{
			if(!SUCCEEDED(m_joystick->GetDeviceState(sizeof(DIJOYSTATE2), &m_joystickState)))
			{
				m_joystick->Acquire();
			}

			m_joystickAxis.x = (m_joystickState.lX - 32767) / 32767;
			m_joystickAxis.y = (m_joystickState.lY - 32767) / 32767;
		}

		float currentTime = eagle.getTimer()->getPassedTimeSeconds();

		for(int i = 0; i < 256; ++i)
		{
			if(isKeyPressed((KeyboardKey)i))
			{
				m_keyHoldDuration[i] = 0;
				m_keyHoldStart[i] = currentTime;

				m_keyTapCount[i]++;

				m_keyTotalPressCount[i]++;

				addToKeySequence(i);
			}
			else if(isKeyDown((KeyboardKey)i))
			{
				m_keyHoldDuration[i] = currentTime - m_keyHoldStart[i];
			}
			else
			{
				m_keyHoldDuration[i] = 0;
			}
		}

		for(int i = 0; i < 4; ++i)
		{
			if(isMouseButtonPressed((MouseButton)i))
			{
				m_buttonHoldDuration[i] = 0;
				m_buttonHoldStart[i] = currentTime;

				m_buttonTapCount[i]++;

				m_buttonTotalPressCount[i]++;
			}
			else if(isMouseButtonDown((MouseButton)i))
			{
				m_buttonHoldDuration[i] = currentTime - m_buttonHoldStart[i];
			}
			else
			{
				m_buttonHoldDuration[i] = 0;
			}
		}

		if(m_tapTimer.stopWatch(1))
		{
			for(int i = 0; i < 256; ++i)
			{
				m_keyTapFrequency[i] = m_keyTapCount[i];

				m_keyTapCount[i] = 0;
			}

			for(int i = 0; i < 4; ++i)
			{
				m_buttonTapFrequency[i] = m_buttonTapCount[i];

				m_buttonTapCount[i] = 0;
			}

			m_tapTimer.reset();
		}

#else
	
		int currentTime = eagle.getTimer()->getPassedTimeSeconds();

		for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
		{
			if(isMouseButtonPressed(i))
			{
				m_touchHoldDuration[i] = 0;
				m_touchHoldStart[i] = currentTime;

				m_touchTapCount[i]++;

				m_touchTotalPressCount[i]++;
			}
			else if(isMouseButtonDown(i))
			{
				m_touchHoldDuration[i] = currentTime - m_touchHoldStart[i];
			}
			else
			{
				m_touchHoldDuration[i] = 0;

				m_touchMoveList[i].set(0, 0);
			}
		}

		if(m_tapTimer.stopWatch(1))
		{
			for(int i = 0; i < 4; ++i)
			{
				m_touchTapFrequency[i] = m_touchTapCount[i];

				m_touchTapCount[i] = 0;
			}

			m_tapTimer.reset();
		}
#endif
	}

	bool InputSystem::isKeyDown(KeyboardKey key)
	{
		return (getKeyState(key));
	}

	bool InputSystem::isKeyUp(KeyboardKey key)
	{
		return (!getKeyState(key));
	}

	bool InputSystem::isKeyReleased(KeyboardKey key)
	{
		bool a = 0;
		if(m_oldKeys[key] && isKeyUp(key))
		{
			a = 1;
		}
		else
		{
			a = 0;
		}

		//m_oldKeys[k] = getKeyState(k);

		return a;
	}

	bool InputSystem::isKeyPressed(KeyboardKey key)
	{
		bool a = 0;
		if(!m_oldKeys[key] && isKeyDown(key))
		{
			a = 1;
		}
		else
		{
			a = 0;
		}

		//m_oldKeys[k] = getKeyState(k);

		return a;
	}

#ifndef PLATFORM_WP8

	Vector2 InputSystem::getMousePosition()
	{
		return m_mousePosition;
	}

	bool InputSystem::getMouseButtonState(MouseButton button)
	{
		return(m_mouseState.rgbButtons[button] & 0x80);
	}

	bool InputSystem::isMouseButtonDown(MouseButton button)
	{
		return(getMouseButtonState(button));
	}

	bool InputSystem::isMouseButtonUp(MouseButton button)
	{
		return(!getMouseButtonState(button));
	}

	bool InputSystem::isMouseButtonPressed(MouseButton button)
	{
		bool a = 0;

		if(!m_oldButtons[button] && isMouseButtonDown(button))
		{
			a = 1;
		}
		else
		{
			a = 0;
		}

		return a;
	}

	bool InputSystem::isMouseButtonReleased(MouseButton button)
	{
		bool a = 0;

		if(m_oldButtons[button] && !isMouseButtonDown(button))
		{
			a = 1;
		}
		else
		{
			a = 0;
		}

		return a;
	}

	Vector2 InputSystem::getMouseMove()
	{
		return m_mouseMove;
	}

	float InputSystem::getMouseButtonHoldDuration(MouseButton button)
	{
		return m_buttonHoldDuration[button];
	}

	void InputSystem::resetMouseButtonHoldDuration(MouseButton button)
	{
		m_buttonHoldDuration[button] = 0;
		m_buttonHoldStart[button] = eagle.getTimer()->getPassedTimeSeconds();
	}

	int InputSystem::getMouseButtonTapFrequency(MouseButton button)
	{
		return m_buttonTapFrequency[button];
	}

	void InputSystem::resetMouseButtonTapFrequency(MouseButton button)
	{
		m_buttonTapFrequency[button] = 0;
	}

	int InputSystem::getMouseButtonTotalPressCount(MouseButton button)
	{
		return m_buttonTotalPressCount[button];
	}

#else

	Vector2 InputSystem::getMousePosition(int touchID)
	{
		if(touchID < 0 || touchID > INPUT_SUPPORTED_TOUCH_COUNT - 1)
		{
			for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
			{
				if(m_touchList[i]) return m_touchPositionList[i];
			}

			return Vector2(0, 0);
		}
		else
		{
			return m_touchPositionList[touchID];
		}
	}

	bool InputSystem::getMouseButtonState(int touchID)
	{
		if(touchID < 0 || touchID > INPUT_SUPPORTED_TOUCH_COUNT - 1)
		{
			for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
			{
				if(m_touchList[i])
				{
					return 1;
				}
			}

			return 0;
		}
		else
		{
			return m_touchList[touchID];
		}
	}

	bool InputSystem::isMouseButtonDown(int touchID)
	{
		return (getMouseButtonState(touchID));
	}

	bool InputSystem::isMouseButtonUp(int touchID)
	{
		return (!getMouseButtonState(touchID));
	}

	bool InputSystem::isMouseButtonPressed(int touchID)
	{
		if(touchID < 0 || touchID > INPUT_SUPPORTED_TOUCH_COUNT - 1)
		{
			for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
			{
				if(isMouseButtonPressed(i))
				{
					return 1;
				}
			}

			return 0;
		}

		if(!m_oldTouchList[touchID] && isMouseButtonDown(touchID))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	bool InputSystem::isMouseButtonReleased(int touchID)
	{
		if(touchID < 0 || touchID > INPUT_SUPPORTED_TOUCH_COUNT - 1)
		{
			for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
			{
				if(isMouseButtonReleased(i))
				{
					return 1;
				}
			}

			return 0;
		}

		if(m_oldTouchList[touchID] && !isMouseButtonDown(touchID))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	Vector2 InputSystem::getMouseMove(int touchID)
	{
		if(touchID < 0 || touchID > INPUT_SUPPORTED_TOUCH_COUNT - 1)
		{
			for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
			{
				if(m_touchList[i])
				{
					return m_touchMoveList[i];
				}
			}

			return Vector2(0, 0);
		}

		return m_touchMoveList[touchID];
	}

	int InputSystem::getMouseButtonHoldDuration(int touchID)
	{
		if(touchID < 0 || touchID > INPUT_SUPPORTED_TOUCH_COUNT - 1)
		{
			for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
			{
				if(m_touchList[i])
				{
					return m_touchHoldDuration[i];
				}
			}

			return 0;
		}

		return m_touchHoldDuration[touchID];
	}

	void InputSystem::resetMouseButtonHoldDuration(int touchID)
	{
		if(touchID < 0 || touchID > INPUT_SUPPORTED_TOUCH_COUNT - 1)
		{
			return;
		}

		m_touchHoldDuration[touchID] = 0;
		m_touchHoldStart[touchID] = eagle.getTimer()->getPassedTimeSeconds();
	}

	int InputSystem::getMouseButtonTapFrequency(int touchID)
	{
		if(touchID < 0 || touchID > INPUT_SUPPORTED_TOUCH_COUNT - 1)
		{
			return 0;
		}

		return m_touchTapFrequency[touchID];
	}

	void InputSystem::resetMouseButtonTapFrequency(int touchID)
	{
		if(touchID < 0 || touchID > INPUT_SUPPORTED_TOUCH_COUNT - 1)
		{
			return;
		}

		m_touchTapFrequency[touchID] = 0;
	}

	int InputSystem::getMouseButtonTotalPressCount(int touchID)
	{
		if(touchID < 0 || touchID > INPUT_SUPPORTED_TOUCH_COUNT - 1)
		{
			return 0;
		}

		return m_touchTotalPressCount[touchID];
	}

	bool InputSystem::isBackButtonPressed()
	{
		return m_backButtonPressed;
	}

	void InputSystem::suppressBackButtonHandling()
	{
		m_suppressBackButtonHandler = 1;
	}

	void InputSystem::dontSuppressBackButtonHandling()
	{
		m_suppressBackButtonHandler = 0;
	}

#endif

	bool InputSystem::isJoystickButtonDown(int k)
	{
		return (getJoystickButtonState(k));
	}

	bool InputSystem::isJoystickButtonUp(int k)
	{
		return (!getJoystickButtonState(k));
	}

	bool InputSystem::isJoystickButtonReleased(int k)
	{
		bool a = 0;
		if(m_oldJoystickButtons[k] && isJoystickButtonUp(k))
			a = 1;
		else
			a = 0;

		//oldJoystickButtons[k] = getJoystickButtonState(k);

		return a;
	}

	bool InputSystem::isJoystickButtonPressed(int k)
	{
		bool a = 0;
		if(!m_oldJoystickButtons[k] && isJoystickButtonDown(k))
			a = 1;
		else
			a = 0;

		//oldJoystickButtons[k] = getJoystickButtonState(k);

		return a;
	}

	void InputSystem::resetOldKeys()
	{
#ifndef PLATFORM_WP8
		for(int i = 0; i < 256; ++i)
		{
			m_oldKeys[i] = getKeyState((KeyboardKey)i);
		}

		for(int i = 0; i < 128; ++i)
		{
			m_oldJoystickButtons[i] = getJoystickButtonState(i);
		}

		for(int i = 0; i < 4; ++i)
		{
			m_oldButtons[i] = getMouseButtonState((MouseButton)i);
		}
#else
		for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
		{
			m_oldTouchList[i] = m_touchList[i];
		}
#endif
	}

	float InputSystem::getKeyHoldDuration(KeyboardKey key)
	{
		return m_keyHoldDuration[key];
	}

	void InputSystem::resetKeyHoldDuration(KeyboardKey key)
	{
		m_keyHoldDuration[key] = 0;
		m_keyHoldStart[key] = eagle.getTimer()->getPassedTimeSeconds();
	}

	int InputSystem::getKeyTapFrequency(KeyboardKey key)
	{
		return m_keyTapFrequency[key];
	}

	int InputSystem::getKeyTotalPressCount(KeyboardKey key)
	{
		return m_keyTotalPressCount[key];
	}

	void InputSystem::addToKeySequence(int k)
	{
		for(int i = INPUT_MAX_KEY_SEQUENCE_LENGTH - 1; i > 0; --i)
		{
			m_keySequence[i] = m_keySequence[i - 1];
		}

		m_keySequence[0] = k;
	}

	bool InputSystem::checkForKeySequence(std::string sequence)
	{
		int sequenceLength = sequence.length();

		if(sequenceLength == 0 || sequenceLength > INPUT_MAX_KEY_SEQUENCE_LENGTH)
		{
			return 0;
		}

		for(int i = sequenceLength - 1; i < INPUT_MAX_KEY_SEQUENCE_LENGTH; ++i)
		{
			if(sequence[0] == (char)dikToAscii(m_keySequence[i]))
			{
				bool match = 1;

				for(int j = 1; j < sequenceLength; ++j)
				{
					if(sequence[j] != (char)dikToAscii(m_keySequence[i - j]))
					{
						match = 0;

						break;
					}
				}

				if(match)
				{
					return 1;
				}
			}
		}

		return 0;
	}

	void InputSystem::clearKeySequence()
	{
		for(int i = 0; i < 256; ++i)
		{
			m_keySequence[i] = -1;
		}
	}

	void InputSystem::waitForKeyPress()
	{
		while(1)
		{
			for(int i = 0; i < 256; ++i)
			{
				if(isKeyDown((KeyboardKey)i))
				{
					return;
				}
			}

			for(int i = 0; i < 4; ++i)
			{
				if(isMouseButtonDown((MouseButton)i))
				{
					return;
				}
			}

			eagle.sleep(10);

			eagle.updateEngine();
		}
	}

	bool InputSystem::getKeyState(KeyboardKey key)
	{
		return (m_keyState[key] & 0x80);
	}

	bool InputSystem::getJoystickButtonState(int b)
	{
#ifndef PLATFORM_WP8
		return (m_joystickState.rgbButtons[b] & 0x80);
#else
		return 0;
#endif
	}

	long InputSystem::getMouseWheelMove()
	{
		return m_wheelMove / 120;
	}

	Vector2 InputSystem::getJoystickAxis()
	{
		Vector2 p;
		p.x = m_joystickAxis.x;
		p.y = m_joystickAxis.y;
		
		return p;
	}

	bool InputSystem::isJoystickValid()
	{
		return m_joystickValidity;
	}

	void InputSystem::setInputExclusive()
	{
		m_inputExclusive = 1;
	}

	void InputSystem::setInputNonExclusive()
	{
		m_inputExclusive = 0;
	}

#ifdef PLATFORM_WP8

	bool InputSystem::isGyrometerPresent()
	{
		return m_gyrometerPresent;
	}

	bool InputSystem::isAccelerometerPresent()
	{
		return m_accelerometerPresent;
	}

	bool InputSystem::isInclinometerPresent()
	{
		return m_inclinometerPresent;
	}

	Vector3 InputSystem::getGyrometerData()
	{
		return m_gyrometerData;
	}

	Vector3 InputSystem::getAccelerometerData()
	{
		return m_accelerometerData;
	}

	Vector3 InputSystem::getInclinometerData()
	{
		return m_inclinometerData;
	}

#endif

	void InputSystem::resetKeyStates()
	{
		for(int i = 0; i < 256; ++i)
		{
			m_oldKeys[i] = 0;
			m_keyState[i] = 0;

			m_keyHoldDuration[i] = 0;
			m_keyHoldStart[i] = 0;

			m_keyTapCount[i] = 0;
			m_keyTapFrequency[i] = 0;

			m_keyTotalPressCount[i] = 0;

			m_keySequence[i] = -1;
		}
	}
};