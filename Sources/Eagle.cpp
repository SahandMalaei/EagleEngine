#include "Eagle.h"

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

		versionMajor = EAGLE_ENGINE_VERSION_MAJOR;
		versionMinor = EAGLE_ENGINE_VERSION_MINOR;
		revision = EAGLE_ENGINE_VERSION_REVISION;

		gameOver = 0;

		setAppTitle("Project Eagle");

		windowHandle = 0;

		frameTime = 0;

		logStringList.reserve(256);

		errorEncountered = 0;

		loggingSystemEnabled = 1;
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

	std::string EagleEngine::getVersionText()
	{
		std::ostringstream s;

		s << "Eagle Engine v" << versionMajor << "." << versionMinor << "." << revision;

		return s.str();
	}

	void EagleEngine::message(std::string messageString, std::string title)
	{
#ifndef PLATFORM_WP8
		wchar_t messageBuffer[512];
		AnsiToUnicode((char *)messageString.c_str(), messageBuffer, 512);

		wchar_t titleBuffer[512];
		AnsiToUnicode((char *)title.c_str(), titleBuffer, 512);

		MessageBox(getWindowHandle(), messageBuffer, titleBuffer, MB_OK);
#else
		OutputDebugString((messageString + "\n").c_str());
#endif
	}

	void EagleEngine::error(std::string messageString, std::string title)
	{
#ifdef PLATFORM_WP8
		OutputDebugString((messageString + "\n").c_str());
#endif
		outputLogEvent("");
		if(messageString != "") outputLogEvent((char *)(messageString).c_str());
		if(messageString != "") outputLogEvent("Shutting down...");

		cout << "Error : " << messageString << "\n";

		shutdownLoggingSystem();

		if(initialized)
		{
			console.clear();
			console.print("An error has occurred");
			console.print(messageString);
			console.print("Press any key to continue");

			errorEncountered = 1;

#ifndef PLATFORM_WP8
			input.waitForKeyPress();
#endif

			exit(EXIT_FAILURE);

			shutdown();
		}
		else
		{
			exit(EXIT_FAILURE);
		}
	}

	bool EagleEngine::preinitializeEngine()
	{
		graphics.preinitialize();

		initializeLoggingSystem();

		outputLogEvent((char *)(getVersionText()).c_str());
		outputLogEvent("");

		outputLogEvent("Initialization process started");

		timer.reset();

		return 1;
	}

	int EagleEngine::initializeEngine(int width, int height, bool fullscreen)
	{
#ifndef PLATFORM_WP8
		graphics.m_windowHandle = windowHandle;
#endif
		outputLogEvent("Eagle engine successfully initialized");

		return 1;
	}

	void EagleEngine::initializeMainSystems()
	{
		input.initialize();

		eagle.outputLogEvent("Input system has been initialized");

		gameObjectManager = new GameObjectManagerClass();

		eagle.outputLogEvent("GameObjectManager has been initialized");

		graphics.initialize();

		eagle.outputLogEvent("Graphics system has been initialized");

		resourceManager.initialize();

		eagle.outputLogEvent("ResourceManager has been initialized");

		if(!audio.initialize()) 
		{
			eagle.error("Error initializing the Audio System");
		}

		eagle.outputLogEvent("Audio system has been initialized");

		if(!console.initialize()) 
		{
			eagle.error("Error initializing the Console");
		}

		eagle.outputLogEvent("Drop-down console has been initialized");

		eagle.outputLogEvent("Math system has been initialized");

		eagle.outputLogEvent("EagleScript has been initialized");

		eagle.outputLogEvent("");

		initialized = 1;
	}

	void EagleEngine::shutdown()
	{
		eagle.outputLogEvent("");
		eagle.outputLogEvent("Shutting down...");

		shutdownLoggingSystem();

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

	int dikToAscii(DWORD scancode)
	{
#ifndef PLATFORM_WP8
		static HKL layout = GetKeyboardLayout(0);
		static unsigned char State[256];

		if (GetKeyboardState(State) == 0)
			return 0;

		UINT vk = MapVirtualKeyEx(scancode, 1, layout);
		unsigned short ascii;
		ToAsciiEx(vk, scancode, State, &ascii, 0, layout);
		return ascii % 256;
#else
		return 0;
#endif
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

	/*void EagleEngine::loadPostProcessingEffect(std::string address, std::string technique, int index)
	{
		wchar_t addressBuffer[512];
		AnsiToUnicode((char *)address.c_str(), addressBuffer, 512);

		D3DXCreateEffectFromFile(d3ddev, addressBuffer, 0, 0, D3DXSHADER_DEBUG, 0, &postEffects[index], 0);
		techniqueHandles[index] = postEffects[index]->GetTechniqueByName(technique.c_str());
	}*/

	/*void EagleEngine::setPictureFrameRateToDisplayAdapterDefault()
	{
		D3DDISPLAYMODE dm;
		d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);

		setPictureFrameRate(dm.RefreshRate);
	}

	/*D3DFORMAT EagleEngine::getDisplayColorFormat()
	{
		D3DDISPLAYMODE dm;
		d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);

		return dm.Format;
	}*/

	// Logging system

	void EagleEngine::initializeLoggingSystem()
	{
		loggingSystemInitialized = 1;
	}

	void EagleEngine::shutdownLoggingSystem()
	{
		if(!loggingSystemEnabled || !loggingSystemInitialized)
		{
			return;
		}

#ifndef PLATFORM_WP8

		if(!(logFile = fopen("log.txt", "wt")))
		{
			return;
		}

		for(int i = 0; i < logStringList.size(); ++i)
		{
			fwrite(logStringList[i].c_str(), strlen(logStringList[i].c_str()), 1, logFile);
		}

		fclose(logFile);
#endif

		loggingSystemInitialized = 0;
	}

	void EagleEngine::outputLogEvent(std::string eventMessage)
	{
		if(!loggingSystemEnabled || !loggingSystemInitialized)
		{
			return;
		}

#ifdef PLATFORM_WP8
		this->message(eventMessage);
#else
		//OutputDebugStringA((char *)(eventString + "\n").c_str());
#endif

		eventMessage += '\n';

		logStringList.push_back(eventMessage);
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

	int EagleEngine::getVersionMajor()
	{
		return versionMajor;
	}

	int EagleEngine::getVersionMinor()
	{
		return versionMinor;
	}

	int EagleEngine::getRevision()
	{
		return revision;
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

	void EagleEngine::disableLoggingSystem()
	{
		loggingSystemEnabled = 1;
	}

	void EagleEngine::enableLoggingSystem()
	{
		loggingSystemEnabled = 0;
	}

	bool EagleEngine::isLoggingSystemEnabled()
	{
		return loggingSystemEnabled;
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