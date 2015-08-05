#include "Eagle.h"

#include <thread>

namespace ProjectEagle
{
	EagleEngine::EagleEngine()
	{
		processorSaving = 1;

		initialized = 0;

#ifndef PLATFORM_WP8
		targetPlatform = Platform_Win32;
#else
		targetPlatform = Platform_WindowsPhone;
#endif

		framerate = EAGLE_DEFAULT_FRAME_RATE;
		calculataionFrequency = EAGLE_DEFAULT_FRAME_RATE;

		coreFrameCount = 0;
		coreFrameRate = 0;

		windowHandle = 0;

		pauseMode = 0;

		gameOver = 0;

		setAppTitle("Project Eagle");

		windowHandle = 0;

		frameTime = 0;

		errorEncountered = 0;
	}

	EagleEngine::~EagleEngine()
	{
	}

	PlatformType EagleEngine::getTargetPlatform()
	{
		return targetPlatform;
	}

	void EagleEngine::setTargetPlatform(PlatformType target)
	{
		switch(target)
		{
		case Platform_Win32:
			{
				targetPlatform = target;

				break;
			}

		case Platform_WindowsPhone:
			{
				targetPlatform = target;

				break;
			}
		}
	}

	bool EagleEngine::preinitializeEngine()
	{
		graphics.preinitialize();

		Debug::initializeLoggingSystem();

		Debug::outputLogEvent((char *)("Eagle Engine version " + getVersion()).c_str());
		Debug::outputLogEvent("");

		Debug::outputLogEvent("Initialization process started");

		timer.reset();

		return 1;
	}

	int EagleEngine::initializeEngine(int width, int height, bool fullscreen)
	{

#ifndef PLATFORM_WP8

		graphics.m_windowHandle = windowHandle;

#endif
		Debug::outputLogEvent("Eagle engine successfully initialized");

		return 1;
	}

	void EagleEngine::initializeMainSystems()
	{
		input.initialize();

		Debug::outputLogEvent("Input system has been initialized");

		gameObjectManager = new GameObjectManagerClass();

		Debug::outputLogEvent("GameObjectManager has been initialized");

		graphics.initialize();

		Debug::outputLogEvent("Graphics system has been initialized");

		resourceManager.initialize();

		Debug::outputLogEvent("ResourceManager has been initialized");

		if(!audio.initialize()) 
		{
			Debug::throwError("Error initializing the Audio System");
		}

		Debug::outputLogEvent("Audio system has been initialized");

		if(!console.initialize()) 
		{
			Debug::throwError("Error initializing the Console");
		}

		Debug::outputLogEvent("Drop-down console has been initialized");

		Debug::outputLogEvent("Math system has been initialized");

		Debug::outputLogEvent("EagleScript has been initialized");

		Debug::outputLogEvent("");

		initialized = 1;
	}

	void EagleEngine::shutdown()
	{
		Debug::outputLogEvent("");
		Debug::outputLogEvent("Shutting down...");

		Debug::shutdownLoggingSystem();

		gameOver = 1;

#ifdef PLATFORM_WP8
		exit(EXIT_SUCCESS);
#endif
	}

	void EagleEngine::updateEngine()
	{
		if(frameTimer.stopWatch(1.0 / calculataionFrequency))
		{
			frameTime = frameTimer.getPassedTimeSeconds();

			frameTimer.reset();

			input.update();

			console.update();

			if(!errorEncountered)
			{
				resourceManager.update();

				virtualMachine.updateScripts();

				update(frameTime);

				entitySystem.update(frameTime);
				entitySystem.removeDeadEntities();
			}

			if(!errorEncountered)
			{
				if(input.isKeyReleased(Keyboard_F2))
				{
					graphics.takeScreenshot();
				}

				input.resetOldKeys();

				gameObjectManager->UpdateGameObjects();
				gameObjectManager->RemoveDeadGameObjects();

				audio.update();
			}

			if(graphics.renderStart())
			{
				graphics.setWorldIdentity();

				if(!errorEncountered)
				{
					render();

					gameObjectManager->drawGameObjects();
					entitySystem.render();
				}
				else
				{
					console.setShowing(1);
				}

				BlendMode previousBlendMode = graphics.getBlendMode();
				graphics.setBlendMode(BlendMode_Normal);

				console.draw();

				graphics.setBlendMode(previousBlendMode);

				graphics.renderStop();

				graphics.setIdentity();
			}

			float timeElapsed = frameTimer.getPassedTimeSeconds();
			//sleep(max<float>(0.0, min<float>(0.001, 1.0 / calculataionFrequency - timeElapsed - 0.002)));
		}
		
		if(coreTimer.stopWatch(0.1))
		{
			coreFrameRate = 1.0 / frameTime + 0.01;

			coreTimer.reset();
		}
	}

	void EagleEngine::close()
	{
		shutdown();
	}

	bool EagleEngine::toggleFullscreen()
	{
		return 1;
	}

	std::string wstrToStr(const std::wstring &wstr)
	{
		std::string strTo;
		char *szTo = new char[wstr.length() + 1];
		szTo[wstr.size()] = '\0';
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
		//MultiByteToWideChar(
		strTo = szTo;
		delete[] szTo;
		return strTo;
	}

	std::wstring strToWstr(const std::string& s)
	{
		int len;
		int slength = (int)s.length() + 1;
		len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0); 
		wchar_t* buf = new wchar_t[len];
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		std::wstring r(buf);
		delete[] buf;
		return r;
	}

	float EagleEngine::getFrameTime()
	{
		return frameTime;
	}

	float *EagleEngine::getFrameTimePointer()
	{
		return &frameTime;
	}

	void EagleEngine::setWindowHandle(HWND hwnd) 
	{
		windowHandle = hwnd;
	}

	HWND EagleEngine::getWindowHandle() 
	{
		return windowHandle;
	}

	Timer *EagleEngine::getTimer()
	{
		return &timer;
	}

	float EagleEngine::getPictureFrameRate()
	{
		return framerate;
	}

	float EagleEngine::getCalculationsRate()
	{
		return calculataionFrequency;
	}

	bool EagleEngine::isPaused()
	{
		return pauseMode;
	}

	void EagleEngine::setPaused(bool value)
	{
		pauseMode = value;
	}

	std::string EagleEngine::getAppTitle()
	{
		return appTitle;
	}

	void EagleEngine::setAppTitle(std::string title)
	{
		appTitle = title;
	}

	std::string EagleEngine::getVersion()
	{
		return EAGLE_ENGINE_VERSION;
	}

	float EagleEngine::getCoreFrameRate()
	{
		return coreFrameRate;
	}

	bool EagleEngine::getProcessorSaving()
	{
		return processorSaving;
	}

	void EagleEngine::setProcessorSaving(bool value)
	{
		processorSaving = value;
	}

	void EagleEngine::sleep(float seconds)
	{
		this_thread::sleep_for(std::chrono::milliseconds((int)(seconds * 1000.0)));
	}

	void EagleEngine::resetFrameTimer()
	{
		frameTimer.reset();
	}

#ifdef PLATFORM_WP8
	void EagleEngine::disableLockScreen()
	{
		if(!initialized) return;

		displayRequest->RequestActive();
	}

	void EagleEngine::enableLockScreen()
	{
		if(!initialized) return;

		displayRequest->RequestRelease();
	}
#endif
};