/*
  ==============================================================================

    SystemTrayDisplay.h
    Created: 20 Mar 2021 1:02:37pm
    Author:  corbin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "CubeCalendarData.h"

//==============================================================================
/*
*/
class SystemTrayDisplay  : public juce::SystemTrayIconComponent
{
public:
    SystemTrayDisplay(CubeCalendarData& calendarData, juce::Component& window);
    ~SystemTrayDisplay() override;
    
    void update();
    
    void mouseDown (const juce::MouseEvent& e) override;
/*
    void paint (juce::Graphics&) override;
    void resized() override;*/

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SystemTrayDisplay);
    static void menuInvocationCallback (int chosenItemID, SystemTrayDisplay*);
    static const void drawImage(juce::Graphics& g, const CubeData& cube);
    CubeCalendarData& data;
    juce::Image icon;
    juce::Graphics iconGraphics;
    juce::Component& mainWindow;
    juce::PopupMenu menu;
};
