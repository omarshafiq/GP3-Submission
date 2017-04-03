// Included headers
#include "../JuceLibraryCode/JuceHeader.h"

// Creates the main component that hosts our child controls and content
Component* createMainContentComponent();

// This file contains the basic startup code for a Juce application
class SphereTimerApplication : public JUCEApplication
{
public:
#pragma region Constructor
	// Initializes a new instance of the SphereTimerApplication class.
	SphereTimerApplication()
	{
	}
#pragma endregion
#pragma region Overrides
	// When another instance of the app is launched while this one is running,
	// this method is invoked, and the commandLine parameter tells you what
	// the other instance's command-line arguments were.
	void anotherInstanceStarted(const String& commandLine) override
	{
	}

	// Gets the name of this application.
	const String getApplicationName() override
	{
		return ProjectInfo::projectName;
	}

	// Gets the current version of this application.
	const String getApplicationVersion() override
	{
		return ProjectInfo::versionString;
	}

	// This method is where you should put your application's initialisation code
	void initialise(const String& commandLine) override
	{
		// Create a new MainWindow object
		p_MainWindow = new MainWindow(getApplicationName());
	}

	// Gets a value indicating whether more than one application instance is allowed.
	bool moreThanOneInstanceAllowed() override
	{
		return true;
	}

	// Add your application's shutdown code here.
	void shutdown() override
	{
		p_MainWindow = nullptr; // (deletes our window)
	}

	// This is called when the app is being asked to quit: you can ignore this
	// request and let the app carry on running, or call quit() to allow the app to close.
	void systemRequestedQuit() override
	{
		quit();
	}
#pragma endregion
#pragma region Sub-classes
	// This class implements the desktop window that contains an instance of
	// our MainContentComponent class.
	class MainWindow : public DocumentWindow
	{
	public:
		// Initializes a new instance of the MainWindow class.
		MainWindow(String name) : DocumentWindow(name, Colours::black, DocumentWindow::allButtons)
		{
			// Initialize window properties
			setContentOwned(createMainContentComponent(), true);
			setResizable(false, false);
			setFullScreen(true);
			setTitleBarHeight(0);
			setVisible(true);
		}

		// This is called when the user tries to close this window. Here, we'll just
		// ask the app to quit when this happens.
		void closeButtonPressed() override
		{
			JUCEApplication::getInstance()->systemRequestedQuit();
		}

		
	private:
		// Add this class to the Juce leak detector
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
	};
#pragma endregion
private:
#pragma region Members
	// Member variables
	ScopedPointer<MainWindow> p_MainWindow;
#pragma endregion
};

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(SphereTimerApplication)
