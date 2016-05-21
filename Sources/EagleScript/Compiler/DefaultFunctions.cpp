#include "../../Eagle.h"

using namespace EagleScript;
using namespace EagleVirtualMachine;
using namespace ProjectEagle;

namespace EagleScriptCompiler
{
	void sinScriptFunction(EagleVirtualMachine::Script *script)
	{
		float number = getScriptParameterAsFloat(0, script);
		//message(FLOAT_TO_STRING(number));

		returnFloatToScript(sin(number), script);
	}

	void cosScriptFunction(EagleVirtualMachine::Script *script)
	{
		float number = getScriptParameterAsFloat(0, script);

		returnFloatToScript(cos(number), script);
	}

	void asinScriptFunction(EagleVirtualMachine::Script *script)
	{
		float number = getScriptParameterAsFloat(0, script);

		returnFloatToScript(asin(number), script);
	}

	void acosScriptFunction(EagleVirtualMachine::Script *script)
	{
		float number = getScriptParameterAsFloat(0, script);

		returnFloatToScript(acos(number), script);
	}

	void tanScriptFunction(EagleVirtualMachine::Script *script)
	{
		float number = getScriptParameterAsFloat(0, script);

		returnFloatToScript(tan(number), script);
	}

	void atanScriptFunction(EagleVirtualMachine::Script *script)
	{
		float number = getScriptParameterAsFloat(0, script);

		returnFloatToScript(atan(number), script);
	}

	void atan2ScriptFunction(EagleVirtualMachine::Script *script)
	{
		float number0 = getScriptParameterAsFloat(0, script);
		float number1 = getScriptParameterAsFloat(1, script);

		returnFloatToScript(atan2(number0, number1), script);
	}

	void cotScriptFunction(EagleVirtualMachine::Script *script)
	{
		float number = getScriptParameterAsFloat(0, script);

		returnFloatToScript(1 / tan(number), script);
	}

	void absScriptFunction(EagleVirtualMachine::Script *script)
	{
		float number = getScriptParameterAsFloat(0, script);

		returnFloatToScript(abs(number), script);
	}

	void sqrtScriptFunction(EagleVirtualMachine::Script *script)
	{
		float number = getScriptParameterAsFloat(0, script);

		returnFloatToScript(sqrt(number), script);
	}

	void randScriptFunction(EagleVirtualMachine::Script *script)
	{
		int number = rand();

		returnIntToScript(number, script);
	}

	void minScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value0 = getScriptParameterAsFloat(0, script);
		float value1 = getScriptParameterAsFloat(1, script);

		if(value0 < value1)
		{
			returnFloatToScript(value0, script);

			return;
		}
		else
		{
			returnFloatToScript(value1, script);

			return;
		}
	}

	void maxScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value0 = getScriptParameterAsFloat(0, script);
		float value1 = getScriptParameterAsFloat(1, script);

		if(value0 > value1)
		{
			returnFloatToScript(value0, script);

			return;
		}
		else
		{
			returnFloatToScript(value1, script);

			return;
		}
	}

	void getHostGlobalScriptFunction(EagleVirtualMachine::Script *script)
	{
		string identifier = getScriptParameterAsString(0, script).c_str();

		std::vector<HostGlobalVariable>::iterator listIterator;
		for(listIterator = virtualMachine.m_hostGlobalVariableList.begin(); listIterator != virtualMachine.m_hostGlobalVariableList.end(); ++listIterator)
		{
			if((*listIterator).identifier == identifier)
			{
				if((*listIterator).identifier == identifier)
				{
					switch((*listIterator).type)
					{
					case HostGlobalVariable_Int:
						returnIntToScript(*(int *)((*listIterator).variablePointer), script);

						return;

						break;

					case HostGlobalVariable_Float:
						returnFloatToScript(*(float *)((*listIterator).variablePointer), script);

						return;

						break;

					case HostGlobalVariable_String:
						returnStringToScript((char *)(*listIterator).variablePointer, script);

						return;

						break;
					}
				}
			}
		}

		returnIntToScript(0, script);
	}

// Console

	void toggleConsoleScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.toggle();
	}

	void showConsoleScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.show();
	}

	void hideConsoleScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.hide();
	}

	void isConsoleShowingScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(console.isShowing(), script);
	}

	void lockConsoleScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.lock();
	}

	void unlockConsoleScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.unlock();
	}

	void isConsoleMainPanelLockedScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(console.isMainPanelLocked(), script);
	}

	void isConsoleInformationPanelLockedScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(console.isInformationPanelLocked(), script);
	}

	void lockConsoleMainPanelScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.lockMainPanel();
	}

	void unlockConsoleMainPanelScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.unlockMainPanel();
	}

	void lockConsoleInformationPanelScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.lockInformationPanel();
	}

	void unlockConsoleInformationPanelScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.unlockInformationPanel();
	}

	void printScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.print(getScriptParameterAsString(0, script));
	}

	void clearConsoleScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.clear();
	}

	void printEngineVersionScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.printEngineVersion();
	}

	void printFramerateScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.printFrameRate();
	}

	void printDisplayAdapterNameScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.printDisplayAdapterName();
	}

	void printAudioDriverNameScriptFunction(EagleVirtualMachine::Script *script)
	{
		console.printAudioDriverName();
	}

// Eagle

	void sleepScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifndef PLATFORM_WP8
		Sleep(getScriptParameterAsInt(0, script));
#endif
	}

	void messageScriptFunction(EagleVirtualMachine::Script *script)
	{
		Debug::throwMessage(getScriptParameterAsString(0, script));
	}

	void errorScriptFunction(EagleVirtualMachine::Script *script)
	{
		Debug::throwError(getScriptParameterAsString(0, script));
	}

	void outputLogEventScriptFunction(EagleVirtualMachine::Script *script)
	{
		Debug::outputLogEvent((char *)getScriptParameterAsString(0, script).c_str());
	}

	void shutdownScriptFunction(EagleVirtualMachine::Script *script)
	{
		eagle.shutdown();
	}

	void getStringLengthScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(getScriptParameterAsString(0, script).length(), script);
	}

	void getSubstringScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnStringToScript(getScriptParameterAsString(0, script).substr(getScriptParameterAsInt(1, script), getScriptParameterAsInt(2, script)), script);
	}

	void getCharacterScriptFunction(EagleVirtualMachine::Script *script)
	{
		string str = "!";
		str[0] = getScriptParameterAsString(0, script)[getScriptParameterAsInt(1, script)];
		returnStringToScript(str, script);
	}

	void setProcessorSavingScriptFunction(EagleVirtualMachine::Script *script)
	{
		eagle.setProcessorSaving(getScriptParameterAsInt(0, script));
	}

	void getProcessorSavingScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(eagle.getProcessorSaving(), script);
	}

	void getFramerateScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(eagle.getCoreFrameRate(), script);
	}

	void getFrameTimeScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(eagle.getFrameTime(), script);
	}

	void getTimeScriptFunction(EagleVirtualMachine::Script *script)
	{
		LARGE_INTEGER frequency, counter;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&counter);

		returnFloatToScript(counter.LowPart / (float)(frequency.LowPart) * 1000.0, script);
	}

	void getCurrentTickCountScriptFunction(EagleVirtualMachine::Script *script)
	{
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);

		returnFloatToScript(counter.LowPart, script);
	}

	void getProcessorFrequencyScriptFunction(EagleVirtualMachine::Script *script)
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		returnFloatToScript(frequency.LowPart, script);
	}

	void setApplicationTitleScriptFunction(EagleVirtualMachine::Script *script)
	{
		eagle.setAppTitle(getScriptParameterAsString(0, script));
	}

	void getApplicationTitleScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnStringToScript(eagle.getAppTitle(), script);
	}

	void takeScreenshotScriptFunction(EagleVirtualMachine::Script *script)
	{
		graphics.takeScreenshot();
	}

// GraphicsSystem

	void setFullscreenScriptFunction(EagleVirtualMachine::Script *script)
	{
		graphics.setFullscreen(getScriptParameterAsInt(0, script));
	}

	void getFullscreenScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(graphics.getFullscreen(), script);
	}

	void setScreenWidthScriptFunction(EagleVirtualMachine::Script *script)
	{
		graphics.setScreenWidth(getScriptParameterAsInt(0, script));
	}

	void getScreenWidthScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(graphics.getScreenWidth(), script);
	}

	void setScreenHeightScriptFunction(EagleVirtualMachine::Script *script)
	{
		graphics.setScreenHeight(getScriptParameterAsInt(0, script));
	}

	void getScreenHeightScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(graphics.getScreenHeight(), script);
	}

	void setScreenDimensionsScriptFunction(EagleVirtualMachine::Script *script)
	{
		graphics.setScreenDimensions(getScriptParameterAsInt(0, script), getScriptParameterAsInt(1, script));
	}

	void setCursorVisibilityScriptFunction(EagleVirtualMachine::Script *script)
	{
		bool value = getScriptParameterAsInt(0, script);

		graphics.setCursorVisibility(value);
	}

	void isCursorVisibleScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(graphics.getCursorVisibility(), script);
	}

// InputSystem

	void getMousePositionXScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifndef PLATFORM_WP8
		returnFloatToScript(input.getMousePosition().x, script);
#else
		int touchIndex = getScriptParameterAsInt(0, script);

		returnFloatToScript(input.getMousePosition(touchIndex).x, script);
#endif
	}

	void getMousePositionYScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifndef PLATFORM_WP8
		returnFloatToScript(input.getMousePosition().y, script);
#else
		int touchIndex = getScriptParameterAsInt(0, script);

		returnFloatToScript(input.getMousePosition(touchIndex).y, script);
#endif
	}

	void getMouseMoveXScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifndef PLATFORM_WP8
		returnFloatToScript(input.getMouseMove().x, script);
#else
		int touchIndex = getScriptParameterAsInt(0, script);

		returnFloatToScript(input.getMouseMove(touchIndex).x, script);
#endif
	}

	void getMouseMoveYScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifndef PLATFORM_WP8
		returnFloatToScript(input.getMouseMove().y, script);
#else
		int touchIndex = getScriptParameterAsInt(0, script);

		returnFloatToScript(input.getMouseMove(touchIndex).y, script);
#endif
	}

	void getMouseWheelMoveScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(input.getMouseWheelMove(), script);
	}

	void isMouseButtonDownScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.isMouseButtonDown((MouseButton)getScriptParameterAsInt(0, script)), script);
	}

	void isMouseButtonUpScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.isMouseButtonUp((MouseButton)getScriptParameterAsInt(0, script)), script);
	}

	void isMouseButtonReleasedScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.isMouseButtonReleased((MouseButton)getScriptParameterAsInt(0, script)), script);
	}

	void isMouseButtonPressedScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.isMouseButtonPressed((MouseButton)getScriptParameterAsInt(0, script)), script);
	}

	void getMouseButtonHoldDurationScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.getMouseButtonHoldDuration((MouseButton)getScriptParameterAsInt(0, script)), script);
	}

	void resetMouseButtonHoldDurationScriptFunction(EagleVirtualMachine::Script *script)
	{
		input.resetMouseButtonHoldDuration((MouseButton)getScriptParameterAsInt(0, script));
	}

	void getMouseButtonTapFrequencyScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.getMouseButtonTapFrequency((MouseButton)getScriptParameterAsInt(0, script)), script);
	}

	void getMouseButtonTotalPressCountScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.getMouseButtonTotalPressCount((MouseButton)getScriptParameterAsInt(0, script)), script);
	}

	void isKeyDownScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.isKeyDown((KeyboardKey)getScriptParameterAsInt(0, script)), script);
	}

	void isKeyUpScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.isKeyUp((KeyboardKey)getScriptParameterAsInt(0, script)), script);
	}

	void isKeyReleasedScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.isKeyReleased((KeyboardKey)getScriptParameterAsInt(0, script)), script);
	}

	void isKeyPressedScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.isKeyPressed((KeyboardKey)getScriptParameterAsInt(0, script)), script);
	}

	void getKeyHoldDurationScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.getKeyHoldDuration((KeyboardKey)getScriptParameterAsInt(0, script)), script);
	}

	void resetKeyHoldDurationScriptFunction(EagleVirtualMachine::Script *script)
	{
		input.resetKeyHoldDuration((KeyboardKey)getScriptParameterAsInt(0, script));
	}

	void getKeyTapFrequencyScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.getKeyTapFrequency((KeyboardKey)getScriptParameterAsInt(0, script)), script);
	}

	void getKeyTotalPressCountScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(input.getKeyTotalPressCount((KeyboardKey)getScriptParameterAsInt(0, script)), script);
	}

	void isGyrometerPresentScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnIntToScript(input.isGyrometerPresent(), script);
#endif
	}

	void isAccelerometerPresentScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnIntToScript(input.isAccelerometerPresent(), script);
#endif
	}

	void isInclinometerPresentScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnIntToScript(input.isInclinometerPresent(), script);
#endif
	}

	void getGyrometerDataXScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnFloatToScript(input.getGyrometerData().x, script);
#endif
	}

	void getGyrometerDataYScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnFloatToScript(input.getGyrometerData().y, script);
#endif
	}

	void getGyrometerDataZScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnFloatToScript(input.getGyrometerData().z, script);
#endif
	}

	void getAccelerometerDataXScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnFloatToScript(input.getAccelerometerData().x, script);
#endif
	}

	void getAccelerometerDataYScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnFloatToScript(input.getAccelerometerData().y, script);
#endif
	}

	void getAccelerometerDataZScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnFloatToScript(input.getAccelerometerData().z, script);
#endif
	}

	void getInclinometerDataXScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnFloatToScript(input.getInclinometerData().x, script);
#endif
	}

	void getInclinometerDataYScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnFloatToScript(input.getInclinometerData().y, script);
#endif
	}

	void getInclinometerDataZScriptFunction(EagleVirtualMachine::Script *script)
	{
#ifdef PLATFORM_WP8
		returnFloatToScript(input.getInclinometerData().z, script);
#endif
	}

	// AudioSystem

	void loadSoundScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.loadSound2D(getScriptParameterAsString(0, script), getScriptParameterAsString(1, script), getScriptParameterAsString(2, script));
	}

	void loadSound2DScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.loadSound2D(getScriptParameterAsString(0, script), getScriptParameterAsString(1, script), getScriptParameterAsString(2, script));
	}

	void loadSound3DScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.loadSound3D(getScriptParameterAsString(0, script), getScriptParameterAsString(1, script), getScriptParameterAsString(2, script));
	}

	void playSoundScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.playSound(getScriptParameterAsString(0, script));
	}

	void playRandomSoundScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.playRandomSoundFromCategory(getScriptParameterAsString(0, script));
	}

	void stopSoundScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.stopSound(getScriptParameterAsString(0, script));
	}

	void stopAllSoundsScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.stopAllSounds();
	}

	void stopAllSoundsExceptScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.stopAllSoundsExcept(getScriptParameterAsString(0, script));
	}

	void isSoundPlayingScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(audio.isSoundPlaying(getScriptParameterAsString(0, script)), script);
	}

	void soundExistsScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(audio.soundExists(getScriptParameterAsString(0, script)), script);
	}

	void setSoundPanScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setSoundPan(getScriptParameterAsString(0, script), getScriptParameterAsFloat(1, script));
	}

	void getSoundPanScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(audio.getSoundPan(getScriptParameterAsString(0, script)), script);
	}

	void changeSoundPanScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setSoundPan(getScriptParameterAsString(0, script), audio.getSoundPan(getScriptParameterAsString(0, script)) + getScriptParameterAsFloat(1, script));
	}

	void setSoundVolumeScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setSoundVolume(getScriptParameterAsString(0, script), getScriptParameterAsFloat(1, script));
	}

	void getSoundVolumeScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(audio.getSoundVolume(getScriptParameterAsString(0, script)), script);
	}

	void changeSoundVolumeScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setSoundVolume(getScriptParameterAsString(0, script), audio.getSoundVolume(getScriptParameterAsString(0, script)) + getScriptParameterAsFloat(1, script));
	}

	void setSoundFrequencyScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setSoundFrequency(getScriptParameterAsString(0, script), getScriptParameterAsFloat(1, script));
	}

	void getSoundFrequencyScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(audio.getSoundFrequency(getScriptParameterAsString(0, script)), script);
	}

	void changeSoundFrequencyScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setSoundFrequency(getScriptParameterAsString(0, script), audio.getSoundFrequency(getScriptParameterAsString(0, script)) + getScriptParameterAsFloat(1, script));
	}

	void setCameraAsListenerScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setCameraAsListener();
	}

	void disableCameraListenerScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.disableCameraListener();
	}

	void setCameraListenerLockStateScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setCameraListenerLockState(getScriptParameterAsInt(0, script));
	}

	void setListenerPositionScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setListenerPosition(getScriptParameterAsFloat(0, script), getScriptParameterAsFloat(1, script), getScriptParameterAsFloat(2, script));
	}

	void getListenerPositionXScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(audio.getListenerPosition().x, script);
	}

	void getListenerPositionYScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(audio.getListenerPosition().y, script);
	}

	void getListenerPositionZScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(audio.getListenerPosition().z, script);
	}

	void setSoundMinimumDistanceScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setSoundMinimumDistance(getScriptParameterAsString(0, script), getScriptParameterAsFloat(1, script));
	}

	void getSoundMinimumDistanceScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(audio.getSoundMinimumDistance(getScriptParameterAsString(0, script)), script);
	}

	void setSoundMaximumDistanceScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setSoundMaximumDistance(getScriptParameterAsString(0, script), getScriptParameterAsFloat(1, script));
	}

	void getSoundMaximumDistanceScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(audio.getSoundMaximumDistance(getScriptParameterAsString(0, script)), script);
	}

	void setSoundPositionScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setSoundPosition(getScriptParameterAsString(0, script), getScriptParameterAsFloat(1, script), getScriptParameterAsFloat(2, script), getScriptParameterAsFloat(3, script));
	}

	void getSoundPositionXScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(audio.getSoundPosition(getScriptParameterAsString(0, script)).x, script);
	}

	void getSoundPositionYScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(audio.getSoundPosition(getScriptParameterAsString(0, script)).y, script);
	}

	void getSoundPositionZScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnFloatToScript(audio.getSoundPosition(getScriptParameterAsString(0, script)).z, script);
	}

	void setDopplerScaleScriptFunction(EagleVirtualMachine::Script *script)
	{
		audio.setDopplerScale(getScriptParameterAsFloat(0, script));
	}

	void randomScriptFunction(EagleVirtualMachine::Script *script)
	{
		int parameter0, parameter1;
		parameter0 = getScriptParameterAsInt(0, script);
		parameter1 = getScriptParameterAsInt(1, script);

		if(parameter1 == parameter0)
		{
			returnIntToScript(math.random(), script);
		}
		else if(parameter1 < parameter0)
		{
			returnIntToScript(math.random(parameter0), script);
		}
		else
		{
			returnIntToScript(math.random(parameter0, parameter1), script);
		}
	}

	void randomFloatScriptFunction(EagleVirtualMachine::Script *script)
	{
		float parameter0, parameter1;
		parameter0 = getScriptParameterAsFloat(0, script);
		parameter1 = getScriptParameterAsFloat(1, script);

		if(parameter1 == parameter0)
		{
			returnFloatToScript(math.randomFloat(), script);
		}
		else if(parameter1 < parameter0)
		{
			returnFloatToScript(math.randomFloat(parameter0), script);
		}
		else
		{
			returnFloatToScript(math.randomFloat(parameter0, parameter1), script);
		}
	}

	void COLOR_RGBScriptFunction(EagleVirtualMachine::Script *script)
	{
		int r = getScriptParameterAsFloat(0, script);
		int g = getScriptParameterAsFloat(1, script);
		int b = getScriptParameterAsFloat(2, script);

		//returnIntToScript(COLOR_RGB(r, g, b).getDWORD(), script);
	}

	void COLOR_RGBAScriptFunction(EagleVirtualMachine::Script *script)
	{
		int r = getScriptParameterAsFloat(0, script);
		int g = getScriptParameterAsFloat(1, script);
		int b = getScriptParameterAsFloat(2, script);
		int a = getScriptParameterAsFloat(3, script);

		//returnIntToScript(COLOR_RGBA(r, g, b, a).getDWORD(), script);
	}

	// ResourceManager

	void loadTextureScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript((int)resourceManager.loadTexture((char *)getScriptParameterAsString(0, script).c_str()), script);
	}

	// EntitySystem

	void addEntityScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(entitySystem.addEntity(), script);
	}

	void removeEntityScriptFunction(EagleVirtualMachine::Script *script)
	{
		int entityID = getScriptParameterAsInt(0, script);

		entitySystem.removeEntity(entityID);
	}

	void removeAllEntitiesScriptFunction(EagleVirtualMachine::Script *script)
	{
		entitySystem.removeAllEntities();
	}

	void addEntityToGroupScriptFunction(EagleVirtualMachine::Script *script)
	{
		int entityID = getScriptParameterAsInt(0, script);
		int groupID = getScriptParameterAsInt(1, script);

		entitySystem.addEntityToGroup(entityID, groupID);
	}

	void removeEntityFromGroupScriptFunction(EagleVirtualMachine::Script *script)
	{
		int entityID = getScriptParameterAsInt(0, script);
		int groupID = getScriptParameterAsInt(1, script);

		entitySystem.removeEntityFromGroup(entityID, groupID);
	}

	void removeAllEntitiesInGroupScriptFunction(EagleVirtualMachine::Script *script)
	{
		int groupID = getScriptParameterAsInt(0, script);

		entitySystem.removeAllEntitiesInGroup(groupID);
	}

	void removeGroupScriptFunction(EagleVirtualMachine::Script *script)
	{
		int groupID = getScriptParameterAsInt(0, script);

		entitySystem.removeGroup(groupID);
	}

	void isEntityInGroupScriptFunction(EagleVirtualMachine::Script *script)
	{
		int entityID = getScriptParameterAsInt(0, script);
		int groupID = getScriptParameterAsInt(1, script);

		returnIntToScript(entitySystem.isEntityInGroup(entityID, groupID), script);
	}

	void addComponentScriptFunction(EagleVirtualMachine::Script *script)
	{
		int entityID = getScriptParameterAsInt(0, script);
		std::string componentName = getScriptParameterAsString(1, script);

		entitySystem.scriptInterfaceAddComponent(entityID, componentName);
	}

	void setComponentVariableScriptFunction(EagleVirtualMachine::Script *script)
	{
		std::string componentName = getScriptParameterAsString(0, script);
		std::string variableName = getScriptParameterAsString(1, script);

		GameComponentVariableType variableType = entitySystem.getComponentVariableType(componentName, variableName);

		switch(variableType)
		{
		case GameComponentVariable_Bool:
			{
				bool value = getScriptParameterAsInt(2, script);
				entitySystem.scriptInterfaceSetComponentVariableBool(componentName, variableName, value);

				break;
			}

		case GameComponentVariable_Char:
			{
				char value = getScriptParameterAsInt(2, script);
				entitySystem.scriptInterfaceSetComponentVariableChar(componentName, variableName, value);

				break;
			}

		case GameComponentVariable_Short:
			{
				short value = getScriptParameterAsInt(2, script);
				entitySystem.scriptInterfaceSetComponentVariableShort(componentName, variableName, value);

				break;
			}

		case GameComponentVariable_Int:
			{
				int value = getScriptParameterAsInt(2, script);
				entitySystem.scriptInterfaceSetComponentVariableInt(componentName, variableName, value);

				break;
			}

		case GameComponentVariable_Float:
			{
				float value = getScriptParameterAsFloat(2, script);
				entitySystem.scriptInterfaceSetComponentVariableFloat(componentName, variableName, value);

				break;
			}
		}
	}

	void setEntityComponentVariableScriptFunction(EagleVirtualMachine::Script *script)
	{
		int entityID = getScriptParameterAsInt(0, script);
		std::string componentName = getScriptParameterAsString(1, script);
		std::string variableName = getScriptParameterAsString(2, script);

		GameComponentVariableType variableType = entitySystem.getComponentVariableType(componentName, variableName);

		switch(variableType)
		{
		case GameComponentVariable_Bool:
			{
				bool value = getScriptParameterAsInt(3, script);
				entitySystem.scriptInterfaceSetEntityComponentVariableBool(entityID, componentName, variableName, value);

				break;
			}

		case GameComponentVariable_Char:
			{
				char value = getScriptParameterAsInt(3, script);
				entitySystem.scriptInterfaceSetEntityComponentVariableChar(entityID, componentName, variableName, value);

				break;
			}

		case GameComponentVariable_Short:
			{
				short value = getScriptParameterAsInt(3, script);
				entitySystem.scriptInterfaceSetEntityComponentVariableShort(entityID, componentName, variableName, value);

				break;
			}

		case GameComponentVariable_Int:
			{
				int value = getScriptParameterAsInt(3, script);
				entitySystem.scriptInterfaceSetEntityComponentVariableInt(entityID, componentName, variableName, value);

				break;
			}

		case GameComponentVariable_Float:
			{
				float value = getScriptParameterAsFloat(3, script);
				entitySystem.scriptInterfaceSetEntityComponentVariableFloat(entityID, componentName, variableName, value);

				break;
			}
		}
	}

	void getComponentVariableScriptFunction(EagleVirtualMachine::Script *script)
	{
		std::string componentName = getScriptParameterAsString(0, script);
		std::string variableName = getScriptParameterAsString(1, script);

		GameComponentVariableType variableType = entitySystem.getComponentVariableType(componentName, variableName);

		switch(variableType)
		{
		case GameComponentVariable_Bool:
			{
				returnIntToScript(entitySystem.scriptInterfaceGetComponentVariableBool(componentName, variableName), script);

				break;
			}

		case GameComponentVariable_Char:
			{
				returnIntToScript(entitySystem.scriptInterfaceGetComponentVariableChar(componentName, variableName), script);

				break;
			}

		case GameComponentVariable_Short:
			{
				returnIntToScript(entitySystem.scriptInterfaceGetComponentVariableShort(componentName, variableName), script);

				break;
			}

		case GameComponentVariable_Int:
			{
				returnIntToScript(entitySystem.scriptInterfaceGetComponentVariableInt(componentName, variableName), script);

				break;
			}

		case GameComponentVariable_Float:
			{
				returnFloatToScript(entitySystem.scriptInterfaceGetComponentVariableFloat(componentName, variableName), script);

				break;
			}
		}
	}

	void getEntityComponentVariableScriptFunction(EagleVirtualMachine::Script *script)
	{
		int entityID = getScriptParameterAsInt(0, script);
		std::string componentName = getScriptParameterAsString(1, script);
		std::string variableName = getScriptParameterAsString(2, script);

		GameComponentVariableType variableType = entitySystem.getComponentVariableType(componentName, variableName);

		switch(variableType)
		{
		case GameComponentVariable_Bool:
			{
				returnIntToScript(entitySystem.scriptInterfaceGetEntityComponentVariableBool(entityID, componentName, variableName), script);

				break;
			}

		case GameComponentVariable_Char:
			{
				returnIntToScript(entitySystem.scriptInterfaceGetEntityComponentVariableChar(entityID, componentName, variableName), script);

				break;
			}

		case GameComponentVariable_Short:
			{
				returnIntToScript(entitySystem.scriptInterfaceGetEntityComponentVariableShort(entityID, componentName, variableName), script);

				break;
			}

		case GameComponentVariable_Int:
			{
				returnIntToScript(entitySystem.scriptInterfaceGetEntityComponentVariableInt(entityID, componentName, variableName), script);

				break;
			}

		case GameComponentVariable_Float:
			{
				returnFloatToScript(entitySystem.scriptInterfaceGetEntityComponentVariableFloat(entityID, componentName, variableName), script);

				break;
			}
		}
	}

	void saveEntityAsPresetScriptFunction(EagleVirtualMachine::Script *script)
	{
		int entityID = getScriptParameterAsInt(0, script);

		returnIntToScript(entitySystem.saveEntityAsPreset(entityID), script);
	}

	void addEntityFromPresetScriptFunction(EagleVirtualMachine::Script *script)
	{
		int presetID = getScriptParameterAsInt(0, script);

		returnIntToScript(entitySystem.addEntityFromPreset(presetID), script);
	}

	void removePresetScriptFunction(EagleVirtualMachine::Script *script)
	{
		int presetID = getScriptParameterAsInt(0, script);

		entitySystem.removePreset(presetID);
	}

	void removeAllPresetsScriptFunction(EagleVirtualMachine::Script *script)
	{
		entitySystem.removeAllPresets();
	}

	void getEntityCountScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(entitySystem.getEntityCount(), script);
	}

	void getEntityPresetCountScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(entitySystem.getEntityPresetCount(), script);
	}

	void getHighestEntityIDScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(entitySystem.getHighestEntityID(), script);
	}

	void getDeadEntityCountScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(entitySystem.getDeadEntityCount(), script);
	}

	void isEntityPresentScriptFunction(EagleVirtualMachine::Script *script)
	{
		int entityID = getScriptParameterAsInt(0, script);

		returnIntToScript(entitySystem.isEntityPresent(entityID), script);
	}

	void addWorldScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(entitySystem.addScene(), script);
	}

	void removeWorldScriptFunction(EagleVirtualMachine::Script *script)
	{
		int worldIndex = getScriptParameterAsInt(0, script);

		entitySystem.removeScene(worldIndex);
	}

	void getWorldCountScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(entitySystem.getSceneCount(), script);
	}

	void setCurrentWorldScriptFunction(EagleVirtualMachine::Script *script)
	{
		int worldIndex = getScriptParameterAsInt(0, script);

		entitySystem.setCurrentScene(worldIndex);
	}

	void getCurrentWorldIndexScriptFunction(EagleVirtualMachine::Script *script)
	{
		returnIntToScript(entitySystem.getCurrentSceneIndex(), script);
	}

	void sendMessageScriptFunction(EagleVirtualMachine::Script *script)
	{
		int messageCode = getScriptParameterAsInt(0, script);
		int lifeSpan = getScriptParameterAsInt(1, script);
		int delay = getScriptParameterAsInt(2, script);

		entitySystem.addMessage(messageCode, lifeSpan, delay);
	}

	void isMessagePresentScriptFunction(EagleVirtualMachine::Script *script)
	{
		int messageCode = getScriptParameterAsInt(0, script);

		returnIntToScript(entitySystem.isMessagePresent(messageCode), script);
	}

	// Camera

	void setCameraPositionScriptFunction(EagleVirtualMachine::Script *script)
	{
		float x = getScriptParameterAsFloat(0, script);
		float y = getScriptParameterAsFloat(1, script);
		float z = getScriptParameterAsFloat(2, script);

		graphics.getCamera()->setPosition(Vector3(x, y, z));
	}

	void setCameraPositionXScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = getScriptParameterAsFloat(0, script);

		Vector3 newPosition = graphics.getCamera()->getPosition();
		newPosition.x = value;

		graphics.getCamera()->setPosition(newPosition);
	}

	void setCameraPositionYScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = getScriptParameterAsFloat(0, script);

		Vector3 newPosition = graphics.getCamera()->getPosition();
		newPosition.y = value;

		graphics.getCamera()->setPosition(newPosition);
	}

	void setCameraPositionZScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = getScriptParameterAsFloat(0, script);

		Vector3 newPosition = graphics.getCamera()->getPosition();
		newPosition.z = value;

		graphics.getCamera()->setPosition(newPosition);
	}

	void getCameraPositionXScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = graphics.getCamera()->getPosition().x;

		returnFloatToScript(value, script);
	}

	void getCameraPositionYScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = graphics.getCamera()->getPosition().y;

		returnFloatToScript(value, script);
	}

	void getCameraPositionZScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = graphics.getCamera()->getPosition().z;

		returnFloatToScript(value, script);
	}

	void setCameraRotationScriptFunction(EagleVirtualMachine::Script *script)
	{
		float yaw = getScriptParameterAsFloat(0, script);
		float pitch = getScriptParameterAsFloat(1, script);
		float roll = getScriptParameterAsFloat(2, script);

		graphics.getCamera()->setRotation(yaw, pitch, roll);
	}

	void setCameraRotationYawScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = getScriptParameterAsFloat(0, script);

		graphics.getCamera()->setRotationYaw(value);
	}

	void setCameraRotationPitchScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = getScriptParameterAsFloat(0, script);

		graphics.getCamera()->setRotationPitch(value);
	}

	void setCameraRotationRollScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = getScriptParameterAsFloat(0, script);

		graphics.getCamera()->setRotationRoll(value);
	}

	void getCameraRotationYawScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = graphics.getCamera()->getYawRotation();

		returnFloatToScript(value, script);
	}

	void getCameraRotationPitchScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = graphics.getCamera()->getPitchRotation();

		returnFloatToScript(value, script);
	}

	void getCameraRotationRollScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = graphics.getCamera()->getRollRotation();

		returnFloatToScript(value, script);
	}

	void setCameraFOVScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = getScriptParameterAsFloat(0, script);

		graphics.getCamera()->setFOV(value);
	}

	void getCameraFOVScriptFunction(EagleVirtualMachine::Script *script)
	{
		float value = graphics.getCamera()->getFOV();

		returnFloatToScript(value, script);
	}

	// EagleScript

	void loadEagleScriptScriptFunction(EagleVirtualMachine::Script *script)
	{
		Script *newScript = new Script();

		newScript->loadEagleScript((char *)getScriptParameterAsString(0, script).c_str());
	}

	void registerDefaultFunctions()
	{
		EagleVirtualMachine::registerHostFunction(sinScriptFunction, "sin");
		EagleVirtualMachine::registerHostFunction(cosScriptFunction, "cos");
		EagleVirtualMachine::registerHostFunction(asinScriptFunction, "asin");
		EagleVirtualMachine::registerHostFunction(acosScriptFunction, "acos");
		EagleVirtualMachine::registerHostFunction(tanScriptFunction, "tan");
		EagleVirtualMachine::registerHostFunction(atanScriptFunction, "atan");
		EagleVirtualMachine::registerHostFunction(atan2ScriptFunction, "atan2");
		EagleVirtualMachine::registerHostFunction(cotScriptFunction, "cot");
		EagleVirtualMachine::registerHostFunction(absScriptFunction, "abs");

		EagleVirtualMachine::registerHostFunction(sqrtScriptFunction, "sqrt");

		EagleVirtualMachine::registerHostFunction(randScriptFunction, "rand");

		EagleVirtualMachine::registerHostFunction(minScriptFunction, "min");
		EagleVirtualMachine::registerHostFunction(maxScriptFunction, "max");

		EagleVirtualMachine::registerHostFunction(getHostGlobalScriptFunction, "getHostGlobal");

		// Console

		EagleVirtualMachine::registerHostFunction(toggleConsoleScriptFunction, "toggleConsole");
		EagleVirtualMachine::registerHostFunction(showConsoleScriptFunction, "showConsole");
		EagleVirtualMachine::registerHostFunction(hideConsoleScriptFunction, "hideConsole");
		EagleVirtualMachine::registerHostFunction(isConsoleShowingScriptFunction, "isConsoleShowing");
		EagleVirtualMachine::registerHostFunction(lockConsoleScriptFunction, "lockConsole");
		EagleVirtualMachine::registerHostFunction(unlockConsoleScriptFunction, "unlockConsole");
		EagleVirtualMachine::registerHostFunction(isConsoleMainPanelLockedScriptFunction, "isConsoleMainPanelLocked");
		EagleVirtualMachine::registerHostFunction(isConsoleInformationPanelLockedScriptFunction, "isConsoleInformationPanelLocked");
		EagleVirtualMachine::registerHostFunction(lockConsoleMainPanelScriptFunction, "lockConsoleMainPanel");
		EagleVirtualMachine::registerHostFunction(unlockConsoleMainPanelScriptFunction, "unlockConsoleMainPanel");
		EagleVirtualMachine::registerHostFunction(lockConsoleInformationPanelScriptFunction, "lockConsoleInformationPanel");
		EagleVirtualMachine::registerHostFunction(unlockConsoleInformationPanelScriptFunction, "unlockConsoleInformationPanel");

		EagleVirtualMachine::registerHostFunction(printScriptFunction, "print");
		EagleVirtualMachine::registerHostFunction(clearConsoleScriptFunction, "clearConsole");
		EagleVirtualMachine::registerHostFunction(printEngineVersionScriptFunction, "printEngineVersion");
		EagleVirtualMachine::registerHostFunction(printFramerateScriptFunction, "printFramerate");
		EagleVirtualMachine::registerHostFunction(printDisplayAdapterNameScriptFunction, "printDisplayAdapterName");
		EagleVirtualMachine::registerHostFunction(printAudioDriverNameScriptFunction, "printAudioDriverName");

		// String processing

		EagleVirtualMachine::registerHostFunction(getStringLengthScriptFunction, "getStringLength");
		EagleVirtualMachine::registerHostFunction(getSubstringScriptFunction, "getSubstring");
		EagleVirtualMachine::registerHostFunction(getCharacterScriptFunction, "getCharacter");

		// Engine

		EagleVirtualMachine::registerHostFunction(sleepScriptFunction, "sleep");
		EagleVirtualMachine::registerHostFunction(messageScriptFunction, "message");
		EagleVirtualMachine::registerHostFunction(errorScriptFunction, "error");
		EagleVirtualMachine::registerHostFunction(outputLogEventScriptFunction, "outputLogEvent");

		EagleVirtualMachine::registerHostFunction(shutdownScriptFunction, "shutdown");
		EagleVirtualMachine::registerHostFunction(shutdownScriptFunction, "exit");

		EagleVirtualMachine::registerHostFunction(setFullscreenScriptFunction, "setFullscreen");
		EagleVirtualMachine::registerHostFunction(getFullscreenScriptFunction, "getFullscreen");

		EagleVirtualMachine::registerHostFunction(setScreenWidthScriptFunction, "setScreenWidth");
		EagleVirtualMachine::registerHostFunction(getScreenWidthScriptFunction, "getScreenWidth");
		EagleVirtualMachine::registerHostFunction(setScreenHeightScriptFunction, "setScreenHeight");
		EagleVirtualMachine::registerHostFunction(getScreenHeightScriptFunction, "getScreenHeight");
		EagleVirtualMachine::registerHostFunction(setScreenDimensionsScriptFunction, "setScreenDimensions");

		EagleVirtualMachine::registerHostFunction(setCursorVisibilityScriptFunction, "setCursorVisibility");
		EagleVirtualMachine::registerHostFunction(isCursorVisibleScriptFunction, "isCursorVisible");

		EagleVirtualMachine::registerHostFunction(setProcessorSavingScriptFunction, "setProcessorSaving");
		EagleVirtualMachine::registerHostFunction(getProcessorSavingScriptFunction, "getProcessorSaving");

		EagleVirtualMachine::registerHostFunction(getFramerateScriptFunction, "getFramerate");
		EagleVirtualMachine::registerHostFunction(getFrameTimeScriptFunction, "getFrameTime");

		EagleVirtualMachine::registerHostFunction(getTimeScriptFunction, "getTime");
		EagleVirtualMachine::registerHostFunction(getCurrentTickCountScriptFunction, "getCurrentTickCount");
		EagleVirtualMachine::registerHostFunction(getProcessorFrequencyScriptFunction, "getProcessorFrequency");

		EagleVirtualMachine::registerHostFunction(setApplicationTitleScriptFunction, "setApplicationTitle");
		EagleVirtualMachine::registerHostFunction(getApplicationTitleScriptFunction, "getApplicationTitle");

		EagleVirtualMachine::registerHostFunction(takeScreenshotScriptFunction, "takeScreenshot");

		// InputSystem

		EagleVirtualMachine::registerHostFunction(getMousePositionXScriptFunction, "getMousePositionX");
		EagleVirtualMachine::registerHostFunction(getMousePositionYScriptFunction, "getMousePositionY");

		EagleVirtualMachine::registerHostFunction(getMouseMoveXScriptFunction, "getMouseMoveX");
		EagleVirtualMachine::registerHostFunction(getMouseMoveYScriptFunction, "getMouseMoveY");

		EagleVirtualMachine::registerHostFunction(getMouseWheelMoveScriptFunction, "getMouseWheelMove");

		EagleVirtualMachine::registerHostFunction(isMouseButtonDownScriptFunction, "isMouseButtonDown");
		EagleVirtualMachine::registerHostFunction(isMouseButtonUpScriptFunction, "isMouseButtonUp");
		EagleVirtualMachine::registerHostFunction(isMouseButtonReleasedScriptFunction, "isMouseButtonReleased");
		EagleVirtualMachine::registerHostFunction(isMouseButtonPressedScriptFunction, "isMouseButtonPressed");

		EagleVirtualMachine::registerHostFunction(getMouseButtonHoldDurationScriptFunction, "getMouseButtonHoldDuration");
		EagleVirtualMachine::registerHostFunction(resetMouseButtonHoldDurationScriptFunction, "resetMouseButtonHoldDuration");

		EagleVirtualMachine::registerHostFunction(getMouseButtonTapFrequencyScriptFunction, "getMouseButtonTapFrequency");

		EagleVirtualMachine::registerHostFunction(getMouseButtonTotalPressCountScriptFunction, "getMouseButtonTotalPressCount");

		EagleVirtualMachine::registerHostFunction(isKeyDownScriptFunction, "isKeyDown");
		EagleVirtualMachine::registerHostFunction(isKeyUpScriptFunction, "isKeyUp");
		EagleVirtualMachine::registerHostFunction(isKeyReleasedScriptFunction, "isKeyReleased");
		EagleVirtualMachine::registerHostFunction(isKeyPressedScriptFunction, "isKeyPressed");

		EagleVirtualMachine::registerHostFunction(getKeyHoldDurationScriptFunction, "getKeyHoldDuration");
		EagleVirtualMachine::registerHostFunction(resetKeyHoldDurationScriptFunction, "resetKeyHoldDuration");

		EagleVirtualMachine::registerHostFunction(getKeyTapFrequencyScriptFunction, "getKeyTapFrequency");

		EagleVirtualMachine::registerHostFunction(getKeyTapFrequencyScriptFunction, "getKeyTotalPressCount");

		EagleVirtualMachine::registerHostFunction(isGyrometerPresentScriptFunction, "isGyrometerPresent");
		EagleVirtualMachine::registerHostFunction(isAccelerometerPresentScriptFunction, "isAccelerometerPresent");
		EagleVirtualMachine::registerHostFunction(isInclinometerPresentScriptFunction, "isInclinometerPresent");

		EagleVirtualMachine::registerHostFunction(getGyrometerDataXScriptFunction, "getGyrometerDataX");
		EagleVirtualMachine::registerHostFunction(getGyrometerDataYScriptFunction, "getGyrometerDataY");
		EagleVirtualMachine::registerHostFunction(getGyrometerDataZScriptFunction, "getGyrometerDataZ");

		EagleVirtualMachine::registerHostFunction(getAccelerometerDataXScriptFunction, "getAccelerometerDataX");
		EagleVirtualMachine::registerHostFunction(getAccelerometerDataYScriptFunction, "getAccelerometerDataY");
		EagleVirtualMachine::registerHostFunction(getAccelerometerDataZScriptFunction, "getAccelerometerDataZ");

		EagleVirtualMachine::registerHostFunction(getInclinometerDataXScriptFunction, "getInclinometerDataX");
		EagleVirtualMachine::registerHostFunction(getInclinometerDataYScriptFunction, "getInclinometerDataY");
		EagleVirtualMachine::registerHostFunction(getInclinometerDataZScriptFunction, "getInclinometerDataZ");

		// AudioSystem

		EagleVirtualMachine::registerHostFunction(loadSoundScriptFunction, "loadSound");
		EagleVirtualMachine::registerHostFunction(loadSound2DScriptFunction, "loadSound2D");
		EagleVirtualMachine::registerHostFunction(loadSound3DScriptFunction, "loadSound3D");

		EagleVirtualMachine::registerHostFunction(playSoundScriptFunction, "playSound");
		EagleVirtualMachine::registerHostFunction(playRandomSoundScriptFunction, "playRandomSound");

		EagleVirtualMachine::registerHostFunction(stopSoundScriptFunction, "stopSound");
		EagleVirtualMachine::registerHostFunction(stopAllSoundsScriptFunction, "stopAllSounds");
		EagleVirtualMachine::registerHostFunction(stopAllSoundsExceptScriptFunction, "stopAllSoundsExcept");

		EagleVirtualMachine::registerHostFunction(isSoundPlayingScriptFunction, "isSoundPlaying");
		EagleVirtualMachine::registerHostFunction(soundExistsScriptFunction, "soundExists");

		EagleVirtualMachine::registerHostFunction(setSoundPanScriptFunction, "setSoundPan");
		EagleVirtualMachine::registerHostFunction(getSoundPanScriptFunction, "getSoundPan");
		EagleVirtualMachine::registerHostFunction(changeSoundPanScriptFunction, "changeSoundPan");

		EagleVirtualMachine::registerHostFunction(setSoundVolumeScriptFunction, "setSoundVolume");
		EagleVirtualMachine::registerHostFunction(getSoundVolumeScriptFunction, "getSoundVolume");
		EagleVirtualMachine::registerHostFunction(changeSoundVolumeScriptFunction, "changeSoundVolume");

		EagleVirtualMachine::registerHostFunction(setSoundFrequencyScriptFunction, "setSoundFrequency");
		EagleVirtualMachine::registerHostFunction(getSoundFrequencyScriptFunction, "getSoundFrequency");
		EagleVirtualMachine::registerHostFunction(changeSoundFrequencyScriptFunction, "changeSoundFrequency");

		EagleVirtualMachine::registerHostFunction(setCameraAsListenerScriptFunction, "setCameraAsListener");
		EagleVirtualMachine::registerHostFunction(disableCameraListenerScriptFunction, "disableCameraListener");
		EagleVirtualMachine::registerHostFunction(setCameraListenerLockStateScriptFunction, "setCameraListenerLockState");

		EagleVirtualMachine::registerHostFunction(setListenerPositionScriptFunction, "setListenerPosition");
		EagleVirtualMachine::registerHostFunction(getListenerPositionXScriptFunction, "getListenerPositionX");
		EagleVirtualMachine::registerHostFunction(getListenerPositionYScriptFunction, "getListenerPositionY");
		EagleVirtualMachine::registerHostFunction(getListenerPositionZScriptFunction, "getListenerPositionZ");

		EagleVirtualMachine::registerHostFunction(setSoundMinimumDistanceScriptFunction, "setSoundMinimumDistance");
		EagleVirtualMachine::registerHostFunction(getSoundMinimumDistanceScriptFunction, "getSoundMinimumDistance");

		EagleVirtualMachine::registerHostFunction(setSoundMaximumDistanceScriptFunction, "setSoundMaximumDistance");
		EagleVirtualMachine::registerHostFunction(getSoundMaximumDistanceScriptFunction, "getSoundMaximumDistance");

		EagleVirtualMachine::registerHostFunction(setSoundPositionScriptFunction, "setSoundPosition");
		EagleVirtualMachine::registerHostFunction(getSoundPositionXScriptFunction, "getSoundPositionX");
		EagleVirtualMachine::registerHostFunction(getSoundPositionYScriptFunction, "getSoundPositionY");
		EagleVirtualMachine::registerHostFunction(getSoundPositionZScriptFunction, "getSoundPositionZ");

		EagleVirtualMachine::registerHostFunction(setDopplerScaleScriptFunction, "setDopplerScale");

		// Math

		EagleVirtualMachine::registerHostFunction(randomScriptFunction, "random");
		EagleVirtualMachine::registerHostFunction(randomFloatScriptFunction, "randomFloat");

		// Helper

		EagleVirtualMachine::registerHostFunction(COLOR_RGBScriptFunction, "COLOR_RGB");
		EagleVirtualMachine::registerHostFunction(COLOR_RGBAScriptFunction, "COLOR_RGBA");

		// ResourceManager

		EagleVirtualMachine::registerHostFunction(loadTextureScriptFunction, "loadTexture");

		// EntitySystem

		EagleVirtualMachine::registerHostFunction(addEntityScriptFunction, "addEntity");
		EagleVirtualMachine::registerHostFunction(removeEntityScriptFunction, "removeEntity");
		EagleVirtualMachine::registerHostFunction(removeAllEntitiesScriptFunction, "removeAllEntities");

		EagleVirtualMachine::registerHostFunction(addEntityToGroupScriptFunction, "addEntityToGroup");
		EagleVirtualMachine::registerHostFunction(removeEntityFromGroupScriptFunction, "removeEntityFromGroup");
		EagleVirtualMachine::registerHostFunction(removeAllEntitiesInGroupScriptFunction, "removeAllEntitiesInGroup");
		EagleVirtualMachine::registerHostFunction(removeGroupScriptFunction, "removeEntityGroup");
		EagleVirtualMachine::registerHostFunction(isEntityInGroupScriptFunction, "isEntityInGroup");

		EagleVirtualMachine::registerHostFunction(addComponentScriptFunction, "addComponent");
		EagleVirtualMachine::registerHostFunction(setComponentVariableScriptFunction, "setComponentVariable");
		EagleVirtualMachine::registerHostFunction(setEntityComponentVariableScriptFunction, "setEntityComponentVariable");
		EagleVirtualMachine::registerHostFunction(getComponentVariableScriptFunction, "getComponentVariable");
		EagleVirtualMachine::registerHostFunction(getEntityComponentVariableScriptFunction, "getEntityComponentVariable");

		EagleVirtualMachine::registerHostFunction(saveEntityAsPresetScriptFunction, "saveEntityAsPreset");
		EagleVirtualMachine::registerHostFunction(addEntityFromPresetScriptFunction, "addEntityFromPreset");
		EagleVirtualMachine::registerHostFunction(removePresetScriptFunction, "removePreset");
		EagleVirtualMachine::registerHostFunction(removeAllPresetsScriptFunction, "removeAllPresets");

		EagleVirtualMachine::registerHostFunction(getEntityCountScriptFunction, "getEntityCount");
		EagleVirtualMachine::registerHostFunction(getEntityPresetCountScriptFunction, "getEntityPresetCount");

		EagleVirtualMachine::registerHostFunction(getHighestEntityIDScriptFunction, "getHighestEntityID");
		EagleVirtualMachine::registerHostFunction(getDeadEntityCountScriptFunction, "getDeadEntityCount");

		EagleVirtualMachine::registerHostFunction(isEntityPresentScriptFunction, "isEntityPresent");

		EagleVirtualMachine::registerHostFunction(addWorldScriptFunction, "addScene");
		EagleVirtualMachine::registerHostFunction(removeWorldScriptFunction, "removeScene");

		EagleVirtualMachine::registerHostFunction(getWorldCountScriptFunction, "getSceneCount");

		EagleVirtualMachine::registerHostFunction(setCurrentWorldScriptFunction, "setCurrentScene");
		EagleVirtualMachine::registerHostFunction(getCurrentWorldIndexScriptFunction, "getCurrentSceneIndex");

		EagleVirtualMachine::registerHostFunction(sendMessageScriptFunction, "sendMessage");
		EagleVirtualMachine::registerHostFunction(isMessagePresentScriptFunction, "isMessagePresent");

		// Camera

		EagleVirtualMachine::registerHostFunction(setCameraPositionScriptFunction, "setCameraPosition");

		EagleVirtualMachine::registerHostFunction(setCameraPositionXScriptFunction, "setCameraPositionX");
		EagleVirtualMachine::registerHostFunction(setCameraPositionYScriptFunction, "setCameraPositionY");
		EagleVirtualMachine::registerHostFunction(setCameraPositionZScriptFunction, "setCameraPositionZ");

		EagleVirtualMachine::registerHostFunction(getCameraPositionXScriptFunction, "getCameraPositionX");
		EagleVirtualMachine::registerHostFunction(getCameraPositionYScriptFunction, "getCameraPositionY");
		EagleVirtualMachine::registerHostFunction(getCameraPositionZScriptFunction, "getCameraPositionZ");

		EagleVirtualMachine::registerHostFunction(setCameraRotationScriptFunction, "setCameraRotation");

		EagleVirtualMachine::registerHostFunction(setCameraRotationYawScriptFunction, "setCameraRotationYaw");
		EagleVirtualMachine::registerHostFunction(setCameraRotationPitchScriptFunction, "setCameraRotationPitch");
		EagleVirtualMachine::registerHostFunction(setCameraRotationRollScriptFunction, "setCameraRotationRoll");

		EagleVirtualMachine::registerHostFunction(getCameraRotationYawScriptFunction, "getCameraRotationYaw");
		EagleVirtualMachine::registerHostFunction(getCameraRotationPitchScriptFunction, "getCameraRotationPitch");
		EagleVirtualMachine::registerHostFunction(getCameraRotationRollScriptFunction, "getCameraRotationRoll");

		EagleVirtualMachine::registerHostFunction(setCameraFOVScriptFunction, "setCameraFOV");
		EagleVirtualMachine::registerHostFunction(getCameraFOVScriptFunction, "getCameraFOV");

		// EagleScript

		EagleVirtualMachine::registerHostFunction(loadEagleScriptScriptFunction, "loadEagleScript");
	}
};