// Eagle game engine
// Created by Sahand Malaei Balasi

#ifndef _EAGLE_H
#define _EAGLE_H 1

#include <time.h>
#include <string>
#include <list>
#include <vector>

#include <WinSock2.h>
#include <windows.h>

#ifdef PLATFORM_WP8
	#include <wrl/client.h>
	using namespace Microsoft::WRL;

	#include "WP8.h"
#else
	#include "winmain.h"
#endif

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

#include "Debug.h"
#include "Thread.h"
#include "Timer.h"
#include "EagleNet.h"
#include "Helpers.h"
#include "InputSystem.h"
#include "MathSystem.h"
#include "Camera.h"
#include "Audio.h"
#include "FileIO.h"
#include "Resource.h"
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

using namespace ProjectEagle;

const std::string EAGLE_ENGINE_VERSION = "0.4.0.3";

extern bool gameOver;
extern bool preload();
extern bool initialize();
extern void update(float frameTime);
extern void end();
extern void render();

extern WPARAM currentKey;

inline DWORD FtoDW(float f) {return *((DWORD*)&f);}

#ifndef PLATFORM_WP8
	const int EAGLE_DEFAULT_FRAME_RATE = 60;
#else
	const int EAGLE_DEFAULT_FRAME_RATE = 60;
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

		std::string getVersion();

		float getCoreFrameRate();

		bool getProcessorSaving();
		void setProcessorSaving(bool value);

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

	BOOL WINAPI AnsiToUnicode(LPSTR ansiString, LPWSTR unicodeBuffer, DWORD unicodeBufferSize);
	BOOL WINAPI UnicodeToAnsi(LPWSTR unicodeString, LPSTR ansiBuffer, DWORD ansiBufferSize);
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