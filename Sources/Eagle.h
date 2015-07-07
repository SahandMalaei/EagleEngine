// In The Name of God
// Created by Sahand Malaei Balasi

#ifndef _EAGLE_H
#define _EAGLE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>
#include "ctime"
#include "string"
#include "sstream"
#include "cstdlib"
#include "list"

#include <WinSock2.h>

#include <thread>
#include <mutex>

#include "time.h"
#include <iostream>
#include <windows.h>
#include <time.h>
#include <vector>
#include <unordered_map>
#include <map>
#include <fstream>

#ifdef PLATFORM_WP8
	#include <wrl/client.h>
	using namespace Microsoft::WRL;

	#include "WP8.h"
#else
	#include "winmain.h"
#endif

#include <d3d11_1.h>

#ifndef PLATFORM_WP8
	#include <Include/DirectX/D3DX11.h>
	#include <d3dcompiler.h>
	#include <Include/DirectX/dinput.h>
	#include <Include/DirectX/dinputd.h>
#endif

#include <DirectXMath.h>

#ifndef PLATFORM_WP8
	#include <Include/FMOD/Windows/fmod.hpp>
#else
	#include <Include/FMOD/Windows Phone/fmod.hpp>
#endif

#ifndef PLATFORM_WP8
	#include <Include/tbb/tbb.h>

	using namespace tbb;
#endif

#include <Include/Box2D/Box2D.h>

/*#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "lib/x86/d3dx11.lib")
#pragma comment(lib, "fmodex_vc.lib")
#pragma comment(lib, "Box2D.lib")
#pragma comment(lib, "DirectXTK.lib")*/

using namespace std;

#include "Thread.h"
#include "Timer.h"
#include "EagleNet.h"
#include "Helpers.h"
#include "KeysDef.h"
#include "ButtonsDef.h"
#include "InputSystem.h"
#include "MathSystem.h"
#include "Camera.h"
#include "Audio.h"
#include "FileIO.h"
#include "Vertex.h"
#include "Resource.h"
#include "Shader.h"
#include "Graphics.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Sprite.h"
#include "Font.h"
#include "UI.h"
#include "Console.h"
#include "ParticleSystem.h"
#include "Light.h"
#include "Mesh.h"
#include "EagleScript/EagleScript.h"

#include "EntitySystem/EntitySystem.h"
#include "EntitySystem/GameSystems/Common.h"
#include "EntitySystem/GameSystems/DynamicsSystem.h"
#include "EntitySystem/GameSystems/RenderingSystem.h"

using namespace ProjectEagle;

#define EAGLE_ENGINE_VERSION_MAJOR 0
#define EAGLE_ENGINE_VERSION_MINOR 4
#define EAGLE_ENGINE_VERSION_REVISION 0

extern bool gameOver;
extern bool preload();
extern bool initialize();
extern void update(float frameTime);
extern void end();
extern void render();

extern WPARAM currentKey;

inline DWORD FtoDW(float f) {return *((DWORD*)&f);}

#ifndef PLATFORM_WP8
	#define EAGLE_DEFAULT_FRAME_RATE 60
#else
	#define EAGLE_DEFAULT_FRAME_RATE 60
#endif

#define EAGLE_DEFAULT_FRAME_TIME (1.0 / EAGLE_DEFAULT_FRAME_RATE)

#define ARRAY_SIZE(a) \
	((sizeof(a) / sizeof(*(a))) / \
	static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

namespace ProjectEagle
{
	enum PlatformType
	{
		Platform_Win32 = 0,
		Platform_WindowsPhone = 1
	};
	
	int dikToAscii(DWORD scanCode);

	class EagleEngine
	{
#ifdef PLATFORM_WP8
		friend ref class ApplicationWP8;
#endif

	private:
		int versionMajor, versionMinor, revision;

		bool initialized;

		PlatformType targetPlatform;

		HWND windowHandle;

		std::string appTitle;

		bool pauseMode;

		bool processorSaving;

		Timer coreTimer;
		int coreFrameCount;
		float coreFrameRate;

		float framerate;

		float calculataionFrequency;

		float frameTime;

		// Logging system

		FILE *logFile;

		bool loggingSystemEnabled;
		bool loggingSystemInitialized;

		void initializeLoggingSystem();
		void shutdownLoggingSystem();

		std::vector<std::string> logStringList;

		bool errorEncountered;

		Timer timer;
		Timer frameTimer;

#ifdef PLATFORM_WP8
		Windows::System::Display::DisplayRequest^ displayRequest;
#endif

	public:
		EagleEngine();
		~EagleEngine();
		bool preinitializeEngine();
		int initializeEngine(int width, int height, bool fullscreen);
		void initializeMainSystems();
		bool isInitialized(){return initialized;}
		void close();
		void updateEngine();
		void shutdown();

		PlatformType getTargetPlatform();
		void setTargetPlatform(PlatformType target);

		void message(std::string messageString, std::string title = "Project Eagle");
		void error(std::string messageString, std::string title = "ERROR");

		float getPictureFrameRate();
		float getCalculationsRate();

		bool toggleFullscreen();

		bool gameOver;

		bool isPaused();
		void setPaused(bool value);

		void setWindowHandle(HWND hwnd);
		HWND getWindowHandle();

		std::string getAppTitle();
		void setAppTitle(std::string title);

		int getVersionMajor();
		int getVersionMinor();
		int getRevision();
		std::string getVersionText();

		float getCoreFrameRate();

		bool getProcessorSaving();
		void setProcessorSaving(bool value);

		// Logging system

		void outputLogEvent(std::string eventMessage);

		void disableLoggingSystem();
		void enableLoggingSystem();

		bool isLoggingSystemEnabled();

		float getFrameTime();
		float *getFrameTimePointer();

		Timer *getTimer();

		void sleep(float seconds);

		void resetFrameTimer();

		//void enableSplashScreen(Texture *texture, float alphaIncreaseRate);

#ifdef PLATFORM_WP8
		void disableLockScreen();
		void enableLockScreen();
#endif
	};

	std::string wstrToStr(const std::wstring &wstr);
	std::wstring strToWstr(const std::string& s);
};

extern EagleEngine eagle;
extern InputSystem input;
extern AudioSystem audio;
extern GraphicsSystem graphics;
extern MathSystem math;
extern Console console;
extern GameObjectManagerClass *gameObjectManager;
extern ResourceManagerClass resourceManager;
extern GameEntitySystem entitySystem;

#endif