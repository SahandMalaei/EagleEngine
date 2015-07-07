#ifndef _WP8_H
#define _WP8_H 1

#include <memory>
#include <agile.h>

using namespace Platform;

ref class ApplicationWP8 sealed : public Windows::ApplicationModel::Core::IFrameworkView
{
public:
	ApplicationWP8();
	
	// IFrameworkView Methods.
	virtual void Initialize(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView);
	virtual void SetWindow(Windows::UI::Core::CoreWindow^ window);
	virtual void Load(String^ entryPoint);
	virtual void Run();
	virtual void Uninitialize();

protected:
	// Event Handlers.
	void OnActivated(Windows::ApplicationModel::Core::CoreApplicationView^ applicationView, Windows::ApplicationModel::Activation::IActivatedEventArgs^ args);
	void OnSuspending(Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ args);
	void OnResuming(Object^ sender, Object^ args);
	void OnWindowClosed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::CoreWindowEventArgs^ args);
	void OnVisibilityChanged(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::VisibilityChangedEventArgs^ args);
	void OnPointerPressed(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
	void OnPointerMoved(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
	void OnPointerReleased(Windows::UI::Core::CoreWindow^ sender, Windows::UI::Core::PointerEventArgs^ args);
	void OnBackButtonPressed(Platform::Object^ sender, Windows::Phone::UI::Input::BackPressedEventArgs^ args);

private:
	bool windowClosed;
	bool windowVisible;

	Windows::Devices::Sensors::Gyrometer ^gyrometer;
	Windows::Devices::Sensors::Accelerometer ^accelerometer;
	Windows::Devices::Sensors::Inclinometer ^inclinometer;

	void updateSensorData();
};

ref class Direct3DApplicationSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView();
};

#endif