/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CubeCalendarData.h"
#include "MainComponent.h"
#include "SystemTrayDisplay.h"
#include "ConnectionController.h"

//==============================================================================
class CubesApplication  : public juce::JUCEApplication
{
public:
    //==============================================================================
    CubesApplication() {}

    const juce::String getApplicationName() override       { return ProjectInfo::projectName; }
    const juce::String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override             { return true; }
    
    const void onCommit(const juce::MemoryBlock& data, bool hardCommit) {
        connectionController->sendMessage(data);
        if (hardCommit)
            systemTrayDisplay->update();
    }
    const void onDataChange() {
        systemTrayDisplay->update();
    }

    //==============================================================================
    void initialise (const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..
        cubeCalendarData.reset(new CubeCalendarData());

        mainWindow.reset (new MainWindow (getApplicationName(), *cubeCalendarData));
        systemTrayDisplay.reset (new SystemTrayDisplay(*cubeCalendarData, *mainWindow));
        connectionController.reset (new ConnectionController(*cubeCalendarData));
        
        cubeCalendarData->registerCommitCallback(std::bind(&CubesApplication::onCommit, this, std::placeholders::_1, std::placeholders::_2));
        connectionController->registerDataChangeCallback(std::bind(&CubesApplication::onDataChange, this));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..
        cubeCalendarData = nullptr;
        mainWindow = nullptr; // (deletes our window)
        systemTrayDisplay = nullptr; // (deletes our system tray icon)
        connectionController = nullptr;
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }
    
    void suspended() override
    {
        connectionController->closeConnection();
    }
    
    void resumed() override
    {
        connectionController->establishConnection();
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow    : public juce::DocumentWindow
    {
    public:
        MainWindow (juce::String name, CubeCalendarData& data)
            : DocumentWindow (name,
                              juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                          .findColour (juce::ResizableWindow::backgroundColourId),
                              DocumentWindow::closeButton | DocumentWindow::minimiseButton)
        {
            //setUsingNativeTitleBar (true);
            setContentOwned (new MainComponent(data), true);

           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            setResizable (false, false);
            centreWithSize (getWidth(), getHeight());
           #endif

            setVisible (true);
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
        
        void minimiseButtonPressed() override
        {
            setVisible(false);
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<SystemTrayDisplay> systemTrayDisplay;
    std::unique_ptr<CubeCalendarData> cubeCalendarData;
    std::unique_ptr<ConnectionController> connectionController;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (CubesApplication)
