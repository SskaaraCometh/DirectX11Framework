#include "pch.h"
#include "Game.h"

//Common MS name spaces
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;

ref class DX11Framework sealed : public IFrameworkView
{
	bool closeWindow; //Change this to true when its time to shut the window
	Game myGame;
public:
	virtual void Initialize(CoreApplicationView^ appView)
	{
		appView->Activated += ref new TypedEventHandler<CoreApplicationView ^, IActivatedEventArgs ^>(this, &DX11Framework::OnActivated);
		closeWindow = false;
		CoreApplication::Suspending += ref new EventHandler<SuspendingEventArgs ^>(this, &DX11Framework::OnSuspending);
		CoreApplication::Resuming += ref new EventHandler<Platform::Object ^>(this, &DX11Framework::OnResuming);
	}
	
	virtual void SetWindow(CoreWindow^ Window)
	{
		//Allows to set window notifications E.g keystrokes, mouse clicks etc
		Window->PointerPressed += ref new TypedEventHandler<CoreWindow ^, PointerEventArgs ^>(this, &DX11Framework::OnPointerPressed);
		Window->KeyDown += ref new TypedEventHandler<CoreWindow ^, KeyEventArgs ^>(this, &DX11Framework::OnKeyDown);
		Window->Closed += ref new TypedEventHandler<CoreWindow ^, CoreWindowEventArgs ^>(this, &DX11Framework::OnClosed);
	}

	virtual void Load(String^ EntryPoint)
	{
		//Function for graphics
	}

	virtual void Run()
	{
		myGame.Initialise();
		// Most program code goes here
		//----------------ALLOWS FOR WINDOW TO STAY OPEN
		CoreWindow^ Window = CoreWindow::GetForCurrentThread();

		while (!closeWindow)
		{
			//Run processEvents to dispatch events
			//Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessUntilQuit); normal app process
			Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent); // Function returns when all events handled
			myGame.Update();
			myGame.Render();
		}
		
		//-----------------------------------------------
	}

	virtual void Uninitialize()
	{
		//Function for cleanup
	}

	void OnActivated(CoreApplicationView ^sender, IActivatedEventArgs^ args)
	{
		//Obtain pointer to window
		CoreWindow^ Window = CoreWindow::GetForCurrentThread();

		Window->Activate();
	}

	void OnPointerPressed(CoreWindow^ Window, PointerEventArgs^ args)
	{
		MessageDialog Dialog("", "");
		Dialog.Content = "X: " + args->CurrentPoint->Position.X.ToString() + " " + "Y: " + args->CurrentPoint->Position.Y.ToString();
		Dialog.Title = "Notice";
		Dialog.ShowAsync();
	}

	void OnKeyDown(CoreWindow^ Window, KeyEventArgs^ Args)
	{
		MessageDialog Dialog("", "");
		if (Args->VirtualKey == VirtualKey::W)
		{
			Dialog.Content = "Move Forward";
			Dialog.Title = "Notice";
			Dialog.ShowAsync();
		}
		else if (Args->VirtualKey == VirtualKey::A)
		{
			Dialog.Content = "Move Left";
			Dialog.Title = "Notice";
			Dialog.ShowAsync();
		}
		else if (Args->VirtualKey == VirtualKey::S)
		{
			Dialog.Content = "Move Back";
			Dialog.Title = "Notice";
			Dialog.ShowAsync();
		}
		else if (Args->VirtualKey == VirtualKey::D)
		{
			Dialog.Content = "Move Right";
			Dialog.Title = "Notice";
			Dialog.ShowAsync();
		}
	}

	void OnSuspending(Object^ sender, SuspendingEventArgs^ args)
	{

	}

	void OnResuming(Object^ sender, Object^ args)
	{

	}

	void OnClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
	{
		closeWindow = true;
	}
};

ref class AppSource sealed : IFrameworkViewSource
{
public:
	virtual IFrameworkView^ CreateView()
	{
		return ref new DX11Framework();
	}
};

[MTAThreadAttribute] //Define main as a multi-threader apartment function

int main(Array<String^>^ args)
{
	//Runs instance of App Source 
	CoreApplication::Run(ref new AppSource());
	return 0;
}

// 1. COM object is a class/set of classes controlled by an interface. We create an interface and access the object through said interface
// 2. ICOM3D
// 3. COM uses a smart pointer 