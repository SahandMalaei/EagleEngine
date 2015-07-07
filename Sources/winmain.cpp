// Created by Sahand Malaei Balasi

#ifndef PLATFORM_WP8

#include "Eagle.h"

using namespace ProjectEagle;

HINSTANCE hInstancee;
HWND windowHandle, splashScreenHandle;
int nCmdShoww;

EagleEngine eagle;
InputSystem input;
AudioSystem audio;
GraphicsSystem graphics;
MathSystem math;
Console console;
GameObjectManagerClass *gameObjectManager;
ResourceManagerClass resourceManager;
GameEntitySystem entitySystem;

bool gameOver;
bool isEndCalled = 0;

WPARAM currentKey;

LRESULT WINAPI WinProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_QUIT:
	case WM_CLOSE:
	case WM_DESTROY:
		eagle.gameOver = 1;

		if(!isEndCalled)
		{
			end();

			isEndCalled = 1;
		}

		break;

	case WM_SIZE:
		{
			if(wParam != SIZE_MINIMIZED && !graphics.getFullscreen() && graphics.isWindowResizable() && eagle.isInitialized())
			{
				Camera *c = graphics.getCamera();
				c->setAspectRatio((float)(LOWORD(lParam)) / (float)(max<int>(10, HIWORD(lParam))));
				graphics.setScreenWidth(max<int>(1, LOWORD(lParam)));
				graphics.setScreenHeight(max<int>(1, HIWORD(lParam)));

				eagle.updateEngine();
				//console.print("R : " + INT_TO_STRING(graphics.getScreenWidth()) + ", " + INT_TO_STRING(graphics.getScreenHeight()));
			}

			break;
		}

	case WM_MOVING:
		{
			eagle.updateEngine();

			break;
		}

	case WM_KEYDOWN:
		{
			currentKey = wParam;

			break;
		}

		/*case WM_SOCKET:
		{
		eagle.setNetworkMessageAvailablity(1);
		WindowsNetworkMessage wnm;
		wnm.message = msg;
		wnm.wParam = wParam;
		wnm.lParam = lParam;
		eagle.setNetworkMessage(wnm);

		break;
		}*/
	}

	return DefWindowProc(windowHandle, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	srand((unsigned int)time(NULL));
	hInstancee = hInstance;
	nCmdShoww = nCmdShow;
	DWORD dwStyle, dwExStyle;
	RECT windowRectangle;

	//eagle = new EagleEngine();

	if(!eagle.preinitializeEngine())
	{
		eagle.error("Error in game preload");
		return 0;
	}

	initializeEagleScript();

	if(!preload())
	{
		eagle.error("Error in game preload");

		return 0;
	}

	eagle.outputLogEvent("Preload function has returned successfully");

	char title[255];
	sprintf(title, "%s", eagle.getAppTitle().c_str());

	windowRectangle.left = 0l;
	windowRectangle.right = (long)graphics.getScreenWidth();
	windowRectangle.top = 0l;
	windowRectangle.bottom = (long)graphics.getScreenHeight();

	wchar_t *titleBuffer = new wchar_t[512];
	AnsiToUnicode(title, titleBuffer, 512);

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = titleBuffer;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	if(graphics.getFullscreen())
	{
		dwStyle = WINDOW_FULLSCREEN_STYLE;
	}
	else
	{
		dwStyle = WINDOW_WINDOWED_STYLE;

		if(graphics.isWindowResizable())
		{
			dwStyle |= WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		}
	}

	ShowCursor(0);

	AdjustWindowRect(&windowRectangle, dwStyle, 0);

	windowHandle = CreateWindow(titleBuffer, titleBuffer, dwStyle, 0, 0, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, 0, 0, hInstancee, 0);
	if(!windowHandle)
	{
		eagle.error("Error creating program window");

		return 0;
	}

	eagle.setWindowHandle(windowHandle);

	ShowWindow(windowHandle, nCmdShoww);
	UpdateWindow(windowHandle);

	eagle.outputLogEvent("Window creation successful");

	if(!eagle.initializeEngine(graphics.getScreenWidth(), graphics.getScreenHeight(), graphics.getFullscreen()))
	{
		eagle.error("Error initializing the Engine");

		return 0;
	}

	eagle.initializeMainSystems();

	graphics.clearScene(Color_Black);

	if(!initialize())
	{
		eagle.error("Initialization was not successful");

		return 0;
	}

	eagle.outputLogEvent("Initialize function returned successfully");

	ShowCursor(graphics.getCursorVisibility());


	while(!eagle.gameOver)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		eagle.updateEngine();
	}

	if(!isEndCalled)
	{
		end();

		isEndCalled = 1;
	}

	eagle.shutdown();

	if(graphics.getFullscreen())
	{
		ShowCursor(1);
	}

	return EXIT_SUCCESS;
}

#endif