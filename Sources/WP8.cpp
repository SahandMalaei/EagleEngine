#ifdef PLATFORM_WP8

#include "Eagle.h"

using namespace ProjectEagle;

EagleEngine eagle;
InputSystem input;
AudioSystem audio;
GraphicsSystem graphics;
MathSystem math;
Console console;
GameObjectManagerClass *gameObjectManager;
ResourceManagerClass resourceManager;
GameEntitySystem entitySystem;

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::Phone::UI::Input;
using namespace Windows::Phone::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;

using namespace Windows::Devices::Sensors;

//using namespace concurrency;

ApplicationWP8::ApplicationWP8() :
	windowClosed(0),
	windowVisible(1)
{
}

void ApplicationWP8::Initialize(CoreApplicationView^ applicationView)
{
	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &ApplicationWP8::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &ApplicationWP8::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Object^>(this, &ApplicationWP8::OnResuming);

	if(!eagle.preinitializeEngine())
	{
		eagle.error("Error in game preload!");

		return;
	}

	initializeEagleScript();

	if(!preload())
	{
		eagle.error("Error in game preload!");

		return;
	}

	eagle.outputLogEvent("Preload function has returned successfully");
}

void ApplicationWP8::SetWindow(CoreWindow^ window)
{
	graphics.m_windowHandle = window;

	window->VisibilityChanged += ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &ApplicationWP8::OnVisibilityChanged);

	window->Closed += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &ApplicationWP8::OnWindowClosed);

	window->PointerPressed += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &ApplicationWP8::OnPointerPressed);

	window->PointerMoved += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &ApplicationWP8::OnPointerMoved);

	window->PointerReleased += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &ApplicationWP8::OnPointerReleased);

	HardwareButtons::BackPressed += ref new EventHandler<BackPressedEventArgs^>(this, &ApplicationWP8::OnBackButtonPressed);   

	auto settings = ref new Windows::UI::ViewManagement::UISettings();
	graphics.m_accentColor = settings->UIElementColor(Windows::UI::ViewManagement::UIElementType::AccentColor);

	eagle.outputLogEvent("Window creation successful");

	if(!eagle.initializeEngine(graphics.getScreenWidth(), graphics.getScreenHeight(), graphics.getFullscreen()))
	{
		eagle.error("Error initializing the Engine");

		return;
	}

	eagle.initializeMainSystems();

	gyrometer = Gyrometer::GetDefault();

	if(gyrometer)
	{
		input.m_gyrometerPresent = 1;
	}
	else
	{
		input.m_gyrometerPresent = 0;
	}

	accelerometer = Accelerometer::GetDefault();

	if(accelerometer)
	{
		input.m_accelerometerPresent = 1;
	}
	else
	{
		input.m_accelerometerPresent = 0;
	}

	inclinometer = Inclinometer::GetDefault();

	if(inclinometer)
	{
		input.m_inclinometerPresent = 1;
	}
	else
	{
		input.m_inclinometerPresent = 0;
	}

	eagle.displayRequest = ref new Display::DisplayRequest();

	//window->IsKeyboardInputEnabled = 1;
}

void ApplicationWP8::Load(String^ entryPoint)
{
	if(!initialize())
	{
		eagle.error("Initialize function returned failure");

		return;
	}

	eagle.outputLogEvent("Initialize function returned success");
}

void ApplicationWP8::Run()
{
	while(!windowClosed)
	{
		if(windowVisible)
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			updateSensorData();

			eagle.updateEngine();

			input.m_backButtonPressed = 0;
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

void ApplicationWP8::Uninitialize()
{
	eagle.shutdownLoggingSystem();
}

void ApplicationWP8::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	windowVisible = args->Visible;
}

void ApplicationWP8::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	windowClosed = 1;
}

void ApplicationWP8::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
	int touchID = args->CurrentPoint->PointerId;

	int index = -1;

	for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
	{
		if(!input.m_touchList[i])
		{
			index = i;

			input.m_touchIDList[i] = touchID;

			break;
		}
	}

	if(index == -1)
	{
		return;
	}

	input.m_touchList[index] = 1;

	input.m_touchPositionList[index].x = graphics.convertDipsToPixels(args->CurrentPoint->Position.X);
	input.m_touchPositionList[index].y = graphics.convertDipsToPixels(args->CurrentPoint->Position.Y);
}

void ApplicationWP8::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
	int touchID = args->CurrentPoint->PointerId;

	int index = -1;

	for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
	{
		if(input.m_touchIDList[i] == touchID)
		{
			index = i;

			break;
		}
	}

	if(index == -1) return;

	input.m_touchList[index] = 1;

	Vector2 currentPosition = Vector2(graphics.convertDipsToPixels(args->CurrentPoint->Position.X), graphics.convertDipsToPixels(args->CurrentPoint->Position.Y));

	input.m_touchMoveList[index] = currentPosition - input.m_touchPositionList[index];

	input.m_touchPositionList[index] = currentPosition;
}

void ApplicationWP8::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{
	int touchID = args->CurrentPoint->PointerId;

	int index = -1;

	for(int i = 0; i < INPUT_SUPPORTED_TOUCH_COUNT; ++i)
	{
		if(input.m_touchIDList[i] == touchID)
		{
			index = i;

			break;
		}
	}

	if(index == -1) return;

	input.m_touchList[index] = 0;

	input.m_touchPositionList[index].x = graphics.convertDipsToPixels(args->CurrentPoint->Position.X);
	input.m_touchPositionList[index].y = graphics.convertDipsToPixels(args->CurrentPoint->Position.Y);
}

void ApplicationWP8::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	CoreWindow::GetForCurrentThread()->Activate();
}

void ApplicationWP8::OnSuspending(Object^ sender, SuspendingEventArgs^ args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	//SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();
	//m_renderer->ReleaseResourcesForSuspending();

	graphics.releaseResourcesForSuspending();

	/*create_task([this, deferral]()
	{
		// Insert your code here.

		deferral->Complete();
	});*/
}
 
void ApplicationWP8::OnResuming(Object^ sender, Object^ args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.
	 //m_renderer->CreateWindowSizeDependentResources();

	graphics.createWindowSizeDependentResources();
}

void ApplicationWP8::OnBackButtonPressed(Platform::Object^ sender, Windows::Phone::UI::Input::BackPressedEventArgs^ args)
{
	input.m_backButtonPressed = 1;
	
	if(input.m_suppressBackButtonHandler)
	{
		args->Handled = 1;
	}
	else
	{
		args->Handled = 0;
	}
}

void ApplicationWP8::updateSensorData()
{
	GyrometerReading ^gyrometerReading = gyrometer->GetCurrentReading();

	input.m_gyrometerData.x = gyrometerReading->AngularVelocityX;
	input.m_gyrometerData.y = gyrometerReading->AngularVelocityY;
	input.m_gyrometerData.z = gyrometerReading->AngularVelocityZ;

	AccelerometerReading ^accelerometerReading = accelerometer->GetCurrentReading();

	input.m_accelerometerData.x = accelerometerReading->AccelerationX;
	input.m_accelerometerData.x = accelerometerReading->AccelerationY;
	input.m_accelerometerData.x = accelerometerReading->AccelerationZ;

	InclinometerReading ^inclinometerReading = inclinometer->GetCurrentReading();

	input.m_inclinometerData.x = inclinometerReading->YawDegrees * piOver180;
	input.m_inclinometerData.y = inclinometerReading->PitchDegrees * piOver180;
	input.m_inclinometerData.z = inclinometerReading->RollDegrees * piOver180;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new ApplicationWP8();
}

[Platform::MTAThread]
int main(Array<String^>^)
{
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();
	CoreApplication::Run(direct3DApplicationSource);

	return 0;
}

#endif